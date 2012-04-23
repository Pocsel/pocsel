#ifndef __CLIENT_RESOURCES_Md5ANIMATION_HPP__
#define __CLIENT_RESOURCES_Md5ANIMATION_HPP__

namespace Client { namespace Resources {

    class Md5Animation
    {
    public:
        // The JointInfo stores the information necessary to build the
        // skeletons for each frame
        struct JointInfo
        {
            std::string name;
            int parentID;
            int flags;
            int startIndex;
        };

        struct Bound
        {
            glm::vec3 min;
            glm::vec3 max;
        };

        struct BaseFrame
        {
            glm::vec3 pos;
            glm::quat orient;
        };

        struct FrameData
        {
            int frameID;
            std::vector<float> frameData;
        };

        // A Skeleton joint is a joint of the skeleton per frame
        struct SkeletonJoint
        {
            SkeletonJoint() : parent(-1), pos(0) {}
            SkeletonJoint(BaseFrame const& src) : pos(src.pos), orient(src.orient) {}

            int parent;
            glm::vec3 pos;
            glm::quat orient;
        };

        // A frame skeleton stores the joints of the skeleton for a single frame.
        struct FrameSkeleton
        {
            std::vector<glm::mat4x4> boneMatrices;
            std::vector<SkeletonJoint> joints;
            int frameId;
        };

    protected:
        std::vector<JointInfo> _jointInfos;
        std::vector<Bound> _bounds;
        std::vector<BaseFrame> _baseFrames;
        std::vector<FrameData> _frames;
        std::vector<FrameSkeleton> _skeletons;    // All the skeletons for all the frames

    private:
        unsigned int _md5Version;
        unsigned int _numFrames;
        unsigned int _numJoints;
        unsigned int _framRate;
        unsigned int _numAnimatedComponents;
        float _animDuration;
        float _frameDuration;

    public:
        explicit Md5Animation(boost::filesystem::path const& filePath);
        ~Md5Animation();

        JointInfo const& GetJointInfo(unsigned int index) const
        {
            assert(index < this->_jointInfos.size());
            return this->_jointInfos[index];
        }

        unsigned int GetMd5Version() const { return this->_md5Version; }
        unsigned int GetNumFrames() const { return this->_numFrames; }
        unsigned int GetNumJoints() const { return this->_numJoints; }
        unsigned int GetFramRate() const { return this->_framRate; }
        unsigned int GetNumAnimatedComponents() const { return this->_numAnimatedComponents; }
        float GetAnimDuration() const { return this->_animDuration; }
        float GetFrameDuration() const { return this->_frameDuration; }

        FrameSkeleton const& GetSkeleton(unsigned int idx) const {
            assert(idx < this->_skeletons.size() && "index out of bounds");
            return this->_skeletons[idx];
        }

    protected:
        // Build the frame skeleton for a particular frame
        void _BuildFrameSkeleton(
                std::vector<FrameSkeleton>& skeletons,
                std::vector<JointInfo> const& jointInfo,
                std::vector<BaseFrame> const& baseFrames,
                FrameData const& frameData);
    };

}}

#endif
