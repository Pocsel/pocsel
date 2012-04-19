#ifndef __CLIENT_GAME_MODEL_HPP__
#define __CLIENT_GAME_MODEL_HPP__

namespace Client { namespace Game {

    class Model
    {
    private:
        Resources::Model const& model;
        Md5Animation* animation;

    public:
        Model(Resources::Model const& model);
    };

}}

#endif
