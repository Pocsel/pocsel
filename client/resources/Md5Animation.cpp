#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "client/resources/Md5Animation.hpp"

#include "tools/Math.hpp"

namespace Client { namespace Resources {

    Md5Animation::Md5Animation() :
        _md5Version(0),
        _numFrames(0),
        _numJoints(0),
        _framRate(0),
        _numAnimatedComponents(0),
        _animDuration(0),
        _frameDuration(0),
        _animTime(0)
    {
    }

    Md5Animation::~Md5Animation()
    {
    }

    bool Md5Animation::LoadAnimation(std::string const& filename)
    {
        if (!boost::filesystem::exists(filename))
        {
            Tools::error << "Md5Animation::LoadAnimation: Failed to find file: " << filename << "\n";
            return false;
        }

        boost::filesystem::path filePath = filename;

        std::string param;
        std::string junk;   // Read junk from the file

        boost::filesystem::ifstream file(filename);
        int fileLength = Tools::Filesystem::GetFileLength(file);
        if (fileLength <= 0)
        {
            Tools::error << "Md5Animation::LoadAnimation: file " << filename << " is empty\n";
            return false;
        }

        this->_jointInfos.clear();
        this->_bounds.clear();
        this->_baseFrames.clear();
        this->_frames.clear();
        this->_animatedSkeleton.joints.clear();
        this->_numFrames = 0;

        file >> param;

        while(!file.eof())
        {
            if (param == "Md5Version")
            {
                file >> this->_md5Version;
                if (this->_md5Version != 10)
                {
                    Tools::error << "Md5Animation::LoadAnimation: " << filename << ": Only MD5 version 10 is supported\n";
                    return false;
                }
            }
            else if (param == "commandline")
            {
                file.ignore(fileLength, '\n'); // Ignore everything else on the line
            }
            else if (param == "numFrames")
            {
                file >> this->_numFrames;
                file.ignore(fileLength, '\n');
            }
            else if (param == "numJoints")
            {
                file >> this->_numJoints;
                file.ignore(fileLength, '\n');
            }
            else if (param == "frameRate")
            {
                file >> this->_framRate;
                file.ignore(fileLength, '\n');
            }
            else if (param == "numAnimatedComponents")
            {
                file >> this->_numAnimatedComponents;
                file.ignore(fileLength, '\n');
            }
            else if (param == "hierarchy")
            {
                file >> junk; // read in the '{' character
                for (int i = 0; i < this->_numJoints; ++i)
                {
                    JointInfo joint;
                    file >> joint.name >> joint.parentID >> joint.flags >> joint.startIndex;
                    Tools::Filesystem::RemoveQuotes(joint.name);

                    this->_jointInfos.push_back(joint);

                    file.ignore(fileLength, '\n');
                }
                file >> junk; // read in the '}' character
            }
            else if (param == "bounds")
            {
                file >> junk; // read in the '{' character
                file.ignore(fileLength, '\n');
                for (int i = 0; i < this->_numFrames; ++i)
                {
                    Bound bound;
                    file >> junk; // read in the '(' character
                    file >> bound.min.x >> bound.min.y >> bound.min.z;
                    file >> junk >> junk; // read in the ')' and '(' characters.
                    file >> bound.max.x >> bound.max.y >> bound.max.z;

                    this->_bounds.push_back(bound);

                    file.ignore(fileLength, '\n');
                }

                file >> junk; // read in the '}' character
                file.ignore(fileLength, '\n');
            }
            else if (param == "baseframe")
            {
                file >> junk; // read in the '{' character
                file.ignore(fileLength, '\n');

                for (int i = 0; i < this->_numJoints; ++i)
                {
                    BaseFrame baseFrame;
                    file >> junk;
                    file >> baseFrame.pos.x >> baseFrame.pos.y >> baseFrame.pos.z;
                    file >> junk >> junk;
                    file >> baseFrame.orient.x >> baseFrame.orient.y >> baseFrame.orient.z;
                    file.ignore(fileLength, '\n');

                    this->_baseFrames.push_back(baseFrame);
                }
                file >> junk; // read in the '}' character
                file.ignore(fileLength, '\n');
            }
            else if (param == "frame")
            {
                FrameData frame;
                file >> frame.frameID >> junk; // Read in the '{' character
                file.ignore(fileLength, '\n');

                for (int i = 0; i < this->_numAnimatedComponents; ++i)
                {
                    float frameData;
                    file >> frameData;
                    frame.frameData.push_back(frameData);
                }

                this->_frames.push_back(frame);

                // Build a skeleton for this frame
                this->_BuildFrameSkeleton(this->_skeletons, this->_jointInfos, this->_baseFrames, frame);

                file >> junk; // Read in the '}' character
                file.ignore(fileLength, '\n' );
            }

            file >> param;
        } // while ( !file.eof )

        // Make sure there are enough joints for the animated skeleton.
        this->_animatedSkeleton.joints.assign(this->_numJoints, SkeletonJoint());

        this->_frameDuration = 1.0f / (float)this->_framRate;
        this->_animDuration = ( this->_frameDuration * (float)this->_numFrames );
        this->_animTime = 0.0f;

        if ((int)this->_jointInfos.size() != this->_numJoints)
        {
            Tools::error << "Md5Animation::LoadAnimation: " << filename <<
                ": number of joints not ok. (need " << this->_numJoints << ", has " << this->_jointInfos.size() << ")\n";
            return false;
        }
        if ((int)this->_bounds.size() != this->_numFrames)
        {
            Tools::error << "Md5Animation::LoadAnimation: " << filename <<
                ": number of bounds not ok. (need " << this->_numFrames << ", has " << this->_bounds.size() << ")\n";
            return false;
        }
        if ((int)this->_baseFrames.size() != this->_numJoints)
        {
            Tools::error << "Md5Animation::LoadAnimation: " << filename <<
                ": number of baseFrames not ok. (need " << this->_numJoints << ", has " << this->_baseFrames.size() << ")\n";
            return false;
        }
        if ((int)this->_frames.size() != this->_numFrames)
        {
            Tools::error << "Md5Animation::LoadAnimation: " << filename <<
                ": number of frames not ok. (need " << this->_numFrames << ", has " << this->_frames.size() << ")\n";
            return false;
        }
        if ((int)this->_skeletons.size() != this->_numFrames)
        {
            Tools::error << "Md5Animation::LoadAnimation: " << filename <<
                ": number of skels not ok. (need " << this->_numFrames << ", has " << this->_skeletons.size() << ")\n";
            return false;
        }

        return true;
    }

    void Md5Animation::_BuildFrameSkeleton(FrameSkeletonList& skeletons, JointInfoList const& jointInfos, BaseFrameList const& baseFrames, FrameData const& frameData )
    {
        FrameSkeleton skeleton;

        for (unsigned int i = 0; i < jointInfos.size(); ++i)
        {
            unsigned int j = 0;

            JointInfo const& jointInfo = jointInfos[i];
            // Start with the base frame position and orientation.
            SkeletonJoint animatedJoint = baseFrames[i];

            animatedJoint.parent = jointInfo.parentID;

            if (jointInfo.flags & 1) // Pos.x
            {
                animatedJoint.pos.x = frameData.frameData[jointInfo.startIndex + j++];
            }
            if (jointInfo.flags & 2) // Pos.y
            {
                animatedJoint.pos.y = frameData.frameData[jointInfo.startIndex + j++];
            }
            if (jointInfo.flags & 4) // Pos.x
            {
                animatedJoint.pos.z  = frameData.frameData[jointInfo.startIndex + j++];
            }
            if (jointInfo.flags & 8) // Orient.x
            {
                animatedJoint.orient.x = frameData.frameData[jointInfo.startIndex + j++];
            }
            if (jointInfo.flags & 16) // Orient.y
            {
                animatedJoint.orient.y = frameData.frameData[jointInfo.startIndex + j++];
            }
            if (jointInfo.flags & 32) // Orient.z
            {
                animatedJoint.orient.z = frameData.frameData[jointInfo.startIndex + j++];
            }

            Tools::Math::ComputeQuatW(animatedJoint.orient);

            if (animatedJoint.parent >= 0) // Has a parent joint
            {
                SkeletonJoint& parentJoint = skeleton.joints[animatedJoint.parent];
                glm::vec3 rotPos = parentJoint.orient * animatedJoint.pos;

                animatedJoint.pos = parentJoint.pos + rotPos;
                animatedJoint.orient = parentJoint.orient * animatedJoint.orient;

                animatedJoint.orient = glm::normalize(animatedJoint.orient);
            }

            skeleton.joints.push_back(animatedJoint);
        }

        skeletons.push_back(skeleton);
    }

    void Md5Animation::Update(float deltaTime)
    {
        if (this->_numFrames < 1)
            return;

        this->_animTime += deltaTime;

        while (this->_animTime > this->_animDuration)
            this->_animTime -= this->_animDuration;
        while (this->_animTime < 0.0f)
            this->_animTime += this->_animDuration;

        // Figure out which frame we're on
        float framNum = this->_animTime * (float)this->_framRate;
        int frame0 = (int)std::floor(framNum);
        int frame1 = (int)std::ceil(framNum);
        frame0 = frame0 % this->_numFrames;
        frame1 = frame1 % this->_numFrames;

        float interpolate = std::fmod(this->_animTime, this->_frameDuration ) / this->_frameDuration;

        this->_InterpolateSkeletons(this->_animatedSkeleton, this->_skeletons[frame0], this->_skeletons[frame1], interpolate);
    }

    void Md5Animation::_InterpolateSkeletons(FrameSkeleton& finalSkeleton, FrameSkeleton const& skeleton0, FrameSkeleton const& skeleton1, float interpolate)
    {
        for ( int i = 0; i < this->_numJoints; ++i )
        {
            SkeletonJoint& finalJoint = finalSkeleton.joints[i];
            SkeletonJoint const& joint0 = skeleton0.joints[i];
            SkeletonJoint const& joint1 = skeleton1.joints[i];

            finalJoint.parent = joint0.parent;

            finalJoint.pos = glm::lerp(joint0.pos, joint1.pos, interpolate);
            finalJoint.orient = glm::mix(joint0.orient, joint1.orient, interpolate);
        }
    }

    void Md5Animation::Render()
    {
//        glPointSize(5.0f);
//        glColor3f(1.0f, 0.0f, 0.0f);
//
//        glPushAttrib( GL_ENABLE_BIT );
//
//        glDisable(GL_LIGHTING );
//        glDisable( GL_DEPTH_TEST );
//
//        const SkeletonJointList& joints = m_AnimatedSkeleton.m_Joints;
//
//        // Draw the joint positions
//        glBegin( GL_POINTS );
//        {
//            for ( unsigned int i = 0; i < joints.size(); ++i )
//            {
//                glVertex3fv( glm::value_ptr(joints[i].m_Pos) );
//            }
//        }
//        glEnd();
//
//        // Draw the bones
//        glColor3f( 0.0f, 1.0f, 0.0f );
//        glBegin( GL_LINES );
//        {
//            for ( unsigned int i = 0; i < joints.size(); ++i )
//            {
//                const SkeletonJoint& j0 = joints[i];
//                if ( j0.m_Parent != -1 )
//                {
//                    const SkeletonJoint& j1 = joints[j0.m_Parent];
//                    glVertex3fv( glm::value_ptr(j0.m_Pos) );
//                    glVertex3fv( glm::value_ptr(j1.m_Pos) );
//                }
//            }
//        }
//        glEnd();
//
//        glPopAttrib();
    }

}}
