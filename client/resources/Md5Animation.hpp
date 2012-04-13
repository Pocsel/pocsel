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
        typedef std::vector<JointInfo> JointInfoList;

        struct Bound
        {
            glm::vec3 min;
            glm::vec3 max;
        };
        typedef std::vector<Bound> BoundList;

        struct BaseFrame
        {
            glm::vec3 pos;
            glm::quat orient;
        };
        typedef std::vector<BaseFrame> BaseFrameList;

        struct FrameData
        {
            int frameID;
            std::vector<float> frameData;
        };
        typedef std::vector<FrameData> FrameDataList;

        // A Skeleton joint is a joint of the skeleton per frame
        struct SkeletonJoint
        {
            SkeletonJoint() : parent(-1), pos(0) {}
            SkeletonJoint(BaseFrame const& src) : pos(src.pos), orient(src.orient) {}

            int parent;
            glm::vec3 pos;
            glm::quat orient;
        };
        typedef std::vector<SkeletonJoint> SkeletonJointList;
        typedef std::vector<glm::mat4x4> SkeletonMatrixList;

        // A frame skeleton stores the joints of the skeleton for a single frame.
        struct FrameSkeleton
        {
            SkeletonMatrixList boneMatrices;
            SkeletonJointList joints;
        };
        typedef std::vector<FrameSkeleton> FrameSkeletonList;

    protected:
        JointInfoList _jointInfos;
        BoundList _bounds;
        BaseFrameList _baseFrames;
        FrameDataList _frames;
        FrameSkeletonList _skeletons;    // All the skeletons for all the frames
        FrameSkeleton _animatedSkeleton;

    private:
        int _md5Version;
        int _numFrames;
        int _numJoints;
        int _framRate;
        int _numAnimatedComponents;
        float _animDuration;
        float _frameDuration;
        float _animTime;

    public:
        Md5Animation();
        virtual ~Md5Animation();

        // Load an animation from the animation file
        bool LoadAnimation(boost::filesystem::path const& filePath);
        // Update this animation's joint set.
        void Update(float deltaTime);
        // Draw the animated skeleton
        void Render();


        FrameSkeleton const& GetSkeleton() const { return this->_animatedSkeleton; }
        const SkeletonMatrixList& GetSkeletonMatrixList() const { return this->_animatedSkeleton.boneMatrices; }
        int GetNumJoints() const { return this->_numJoints; }

        JointInfo const& GetJointInfo(unsigned int index) const
        {
            assert(index < this->_jointInfos.size());
            return this->_jointInfos[index];
        }

    protected:
        // Build the frame skeleton for a particular frame
        void _BuildFrameSkeleton(FrameSkeletonList& skeletons, JointInfoList const& jointInfo, BaseFrameList const& baseFrames, FrameData const& frameData);
        void _InterpolateSkeletons(FrameSkeleton& finalSkeleton, FrameSkeleton const& skeleton0, FrameSkeleton const& skeleton1, float fInterpolate);
    };

}}

#endif
