#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "client/resources/Md5Animation.hpp"

#include "tools/Math.hpp"

namespace Client { namespace Resources {

    Md5Animation::~Md5Animation()
    {
    }

    Md5Animation::Md5Animation(boost::filesystem::path const& filePath) :
        _md5Version(0),
        _numFrames(0),
        _numJoints(0),
        _framRate(0),
        _numAnimatedComponents(0),
        _animDuration(0),
        _frameDuration(0)
    {
        if (!boost::filesystem::exists(filePath))
            throw std::runtime_error("Md5Animation::LoadAnimation: Failed to find file: " + filePath.string() + "");

        std::string param;
        std::string junk;   // Read junk from the file

        boost::filesystem::ifstream file(filePath);
        auto fileLength = Tools::Filesystem::GetFileLength(file);
        if (fileLength <= 0)
            throw std::runtime_error("Md5Animation::LoadAnimation: file " + filePath.string() + " is empty");

        this->_jointInfos.clear();
        this->_bounds.clear();
        this->_baseFrames.clear();
        this->_frames.clear();
        this->_numFrames = 0;

        file >> param;

        while(!file.eof())
        {
            if (param == "Md5Version")
            {
                file >> this->_md5Version;
                if (this->_md5Version != 10)
                    throw std::runtime_error("Md5Animation::LoadAnimation: " + filePath.string() + ": Only MD5 version 10 is supported");
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
                for (unsigned int i = 0; i < this->_numJoints; ++i)
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
                for (unsigned int i = 0; i < this->_numFrames; ++i)
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

                for (unsigned int i = 0; i < this->_numJoints; ++i)
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

                for (unsigned int i = 0; i < this->_numAnimatedComponents; ++i)
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

        this->_frameDuration = 1.0f / (float)this->_framRate;
        this->_animDuration = ( this->_frameDuration * (float)this->_numFrames );

        if (this->_jointInfos.size() != this->_numJoints)
            throw std::runtime_error("Md5Animation::LoadAnimation: " + filePath.string() + ": number of joints not ok. (need " + Tools::ToString(this->_numJoints) + ", has " + Tools::ToString(this->_jointInfos.size()) + ")");
        if (this->_bounds.size() != this->_numFrames)
            throw std::runtime_error("Md5Animation::LoadAnimation: " + filePath.string() + ": number of bounds not ok. (need " + Tools::ToString(this->_numFrames) + ", has " + Tools::ToString(this->_bounds.size()) + ")");
        if (this->_baseFrames.size() != this->_numJoints)
            throw std::runtime_error("Md5Animation::LoadAnimation: " + filePath.string() + ": number of baseFrames not ok. (need " + Tools::ToString(this->_numJoints) + ", has " + Tools::ToString(this->_baseFrames.size()) + ")");
        if (this->_frames.size() != this->_numFrames)
            throw std::runtime_error("Md5Animation::LoadAnimation: " + filePath.string() + ": number of frames not ok. (need " + Tools::ToString(this->_numFrames) + ", has " + Tools::ToString(this->_frames.size()) + ")");
        if (this->_skeletons.size() != this->_numFrames)
            throw std::runtime_error("Md5Animation::LoadAnimation: " + filePath.string() + ": number of skels not ok. (need " + Tools::ToString(this->_numFrames) + ", has " + Tools::ToString(this->_skeletons.size()) + ")");
    }

    void Md5Animation::_BuildFrameSkeleton(
            std::vector<FrameSkeleton>& skeletons,
            std::vector<JointInfo> const& jointInfos,
            std::vector<BaseFrame> const& baseFrames,
            FrameData const& frameData )
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

            skeleton.joints.push_back(animatedJoint);
        }

        skeleton.frameId = frameData.frameID;
        skeletons.push_back(skeleton);
    }

}}
