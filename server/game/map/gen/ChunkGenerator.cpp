#include "server/precompiled.hpp"

#include "server/game/map/gen/ChunkGenerator.hpp"
#include "server/game/map/gen/CubeSpawnInfo.hpp"
#include "server/game/map/gen/Perlin.hpp"
#include "server/game/map/gen/RandMersenneTwister.hpp"

#include "tools/SimpleMessageQueue.hpp"

#include "server/Logger.hpp"

#include "server/game/map/Conf.hpp"
#include "server/game/map/CubeType.hpp"

#include "server/game/map/gen/equations/Equations.hpp"
#include "server/game/map/gen/validators/Validators.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen {

    ChunkGenerator::ChunkGenerator() :
        _messageQueue(new Tools::SimpleMessageQueue(boost::thread::hardware_concurrency() + 1))
        //_messageQueue(new Tools::SimpleMessageQueue(1))
    {
    }

    ChunkGenerator::~ChunkGenerator()
    {
        Tools::debug << "ChunkGenerator::~ChunkGenerator()\n";

        Tools::Delete(this->_messageQueue);

        std::for_each(this->_equations.begin(), this->_equations.end(), [](IEquation* eq) { Tools::Delete(eq); });

        Tools::Delete(this->_perlin);
    }

    void ChunkGenerator::Start(Conf const& conf)
    {
        Tools::debug << "ChunkGenerator::ChunkGenerator()\n";
        Log::load << "Loading chunk generator:\n" <<
                     "name: " << conf.name << "\n" <<
                     "fullname: " << conf.fullname << "\n" <<
                     "is_default: " << conf.is_default << "\n";

        std::map<std::string, Chunk::CubeType> typeIds;

        for (auto it = conf.cubes.begin(), ite = conf.cubes.end() ; it != ite ; ++it)
        {
            Log::load << it->second.type->name << ": " << it->second.type->id << "\n";

            Log::load << "validation_blocs:\n";
            // parcours des ValidationBlocConf
            for (auto it2 = it->second.validation_blocs.begin(),
                    ite2 = it->second.validation_blocs.end(); it2 != ite2; ++it2)
            {
                Log::load << "priority=" << it2->priority << "\n";
                // parcours des ValidatorConf
                for (auto it3 = it2->validators.begin(), ite3 = it2->validators.end(); it3 != ite3; ++it3)
                {
                    Log::load << "---\n   equation=" <<
                        it3->equation << "\n   validator=" << it3->validator << "\n   values=\n";
                    // parcours des values
                    for (auto it4 = it3->values.begin(), ite4 = it3->values.end(); it4 != ite4; ++it4)
                    {
                        Log::load << "      " << it4->first << "=" << it4->second << "\n";
                    }
                    Log::load << "---\n";
                }
            }
        }

        for (auto it = conf.equations.begin(), ite = conf.equations.end() ; it != ite ; ++it)
        {
            Log::load << "equation " << it->first << ":\n";
            Log::load << "function_name=" << it->second.function_name << "\n";
            for (auto it2 = it->second.values.begin() , ite2 = it->second.values.end() ; it2 != ite2 ; ++it2)
            {
                Log::load << it2->first << "= " << it2->second << "\n";
            }
        }

        RandMersenneTwister r(42);
        this->_perlin = new Perlin(r);


        Equations::Equations equationsCreator(*this->_perlin);

        std::map<std::string, IEquation*> equations;

        // remplir la map equations
        for (auto it = conf.equations.begin(), ite = conf.equations.end() ; it != ite ; ++it)
        {
            if (equations.find(it->first) != equations.end())
                Log::load << "WARNING: equations '" << it->first << "' defined 2 times\n";
            else
            {
                IEquation* new_eq = equationsCreator.CreateEquation(it->second.function_name, it->second.values);
                if (new_eq == 0)
                    Log::load << "WARNING: equation '" << it->first << "' could not be created. (" <<
                        it->second.function_name << ")\n";
                else
                    equations[it->first] = new_eq;
            }
        }

        std::map<std::string, int> equations_done;

        // init tout à -1
        for (auto it = equations.begin(), ite = equations.end() ; it != ite ; ++it)
            equations_done[it->first] = -1;


        std::list<Conf::ValidationBlocConf const*> validationbloc_confs;

        // remplir cube_confs dans l'ordre des priotités
        for (auto it = conf.cubes.begin(), ite = conf.cubes.end() ; it != ite ; ++it)
        {
            for (auto vbit = it->second.validation_blocs.begin(), vbite = it->second.validation_blocs.end(); vbit != vbite; ++vbit)
            {
                bool inserted = false;
                for (auto ccit = validationbloc_confs.begin(), ccite = validationbloc_confs.end() ; ccit != ccite && !inserted ; ++ccit)
                {
                    if (vbit->priority <= (*ccit)->priority)
                    {
                        validationbloc_confs.insert(ccit, &(*vbit));
                        inserted = true;
                        if ((vbit->priority == (*ccit)->priority) && (vbit->cube_type->id != (*ccit)->cube_type->id))
                        {
                            Log::load << "WARNING: priority of '" << vbit->cube_type->name <<
                                "' is the same as '" << (*ccit)->cube_type->name <<
                                "'\n        -> the first is '" << vbit->cube_type->name << "'\n";
                        }
                    }
                }
                if (!inserted)
                    validationbloc_confs.push_back(&(*vbit));
            }
        }

        Validators::Validators validators;

        // remplir _equations et _cubes

        int cur_equation_idx = 0;
        for (auto vbit = validationbloc_confs.begin(), vbite = validationbloc_confs.end(); vbit != vbite; ++vbit)
        {
            std::vector<CubeSpawnInfo::Validation> validations;
            // parcours des ValidatorConf
            for (auto it = (*vbit)->validators.begin(), ite = (*vbit)->validators.end(); it != ite; ++it)
            {
                Conf::ValidatorConf const& v = *it;
                if (equations_done.find(v.equation) != equations_done.end())
                {
                    IValidator* validator = validators.CreateValidator(v.validator, v.values);
                    if (validator != 0)
                    {
                        int& idx = equations_done[v.equation];
                        if (idx == -1)
                        {
                            idx = cur_equation_idx++;
                            this->_equations.push_back(equations[v.equation]);
                        }

                        // XXX mode Method Ptr
//                        CubeSpawnInfo::Validation validation({*validator, idx});
//                        Tools::Delete(validator);
                        CubeSpawnInfo::Validation validation;
                        validation.equationIndex = idx;
                        validation.validator = validator;
                        validations.push_back(validation);
                    }
                    else
                    {
                        Log::load << "WARNING: validator '" << v.validator
                            << "' does not exist. (cube '" << (*vbit)->cube_type->name << "')\n";
                    }
                }
                else
                {
                    Log::load << "WARNING: equation '" << v.equation
                        << "' does not exist. (cube '" << (*vbit)->cube_type->name << "')\n";
                }
            }

            if (validations.size())
                this->_cubes.push_back(CubeSpawnInfo((*vbit)->cube_type->id, validations));
            else
            {
                Log::load << "WARNING: a validation bloc for cube '" << (*vbit)->cube_type->id << "' is empty.\n";
            }
        }
        // warnings pour les equations non utilisées
        for (auto it = equations_done.begin(), ite = equations_done.end() ; it != ite ; ++it)
        {
            if (it->second == -1)
            {
                Log::load << "WARNING: equation '" << it->first << "' is not used\n";
                Tools::Delete(equations[it->first]);
            }
        }

        Tools::debug << "ChunkGenerator::Start()\n";
        this->_messageQueue->Start();
    }

    void ChunkGenerator::Stop()
    {
        Tools::debug << "ChunkGenerator::Stop()\n";
        this->_messageQueue->Stop();
    }

    void ChunkGenerator::GetChunk(Chunk::IdType id, Callback& callback)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ChunkGenerator::_GetChunk, this, id, callback));
        this->_messageQueue->PushMessage(m);
    }

    void ChunkGenerator::_GetChunk(Chunk::IdType id, Callback& callback)
    {
        Chunk::CoordsType coords = Chunk::IdToCoords(id);
        Uint32 x, y, z;
        double xx, yy, zz;
        double xx0, yy0, zz0;
        bool check;
        Chunk* nuChunk = new Chunk(coords);

        double* results = new double[this->_equations.size() * Common::ChunkSize3];
        double* resultsBase = results;

        CubeSpawnInfo const* csiBase = this->_cubes.data();
        CubeSpawnInfo const* csi;
        CubeSpawnInfo const* csiEnd = this->_cubes.data() + this->_cubes.size();

        CubeSpawnInfo::Validation const* val;
        CubeSpawnInfo::Validation const* valEnd;

        bool* rDone = new bool[this->_equations.size()];

        unsigned int eqSize = (unsigned int)this->_equations.size();

        xx0 = ((int)coords.x - (1 << 21)) * (int)Common::ChunkSize;
        zz0 = ((int)coords.z - (1 << 21)) * (int)Common::ChunkSize;
        yy0 = ((int)coords.y - (1 << 19)) * (int)Common::ChunkSize;
        //xx0 = (double)((int)coords.x - (1 << 21));
        //zz0 = (double)((int)coords.z - (1 << 21));
        //yy0 = (double)((int)coords.y - (1 << 19));

        for (unsigned int i = 0; i < this->_equations.size(); ++i)
            rDone[i] = false;
        unsigned int rDoneCount = eqSize;

        for (x = 0 ; x < Common::ChunkSize ; ++x)
        {
            xx = xx0 + (double)x;// / Common::ChunkSize;
            for (z = 0 ; z < Common::ChunkSize ; ++z)
            {
                zz = zz0 + (double)z;// / Common::ChunkSize;
                for (y = 0 ; y < Common::ChunkSize ; ++y)
                {
                    yy = yy0 + (double)y;// / Common::ChunkSize;

                    for (csi = csiBase; csi != csiEnd; ++csi)
                    {
                        val = csi->GetValBegin();
                        valEnd = csi->GetValEnd();
                        check = true;
                        for (; val != valEnd; ++val)
                        {
                            if (!rDone[val->equationIndex])
                            {
                                this->_equations[val->equationIndex]->Calc(xx0,
                                                                           yy0,
                                                                           zz0,
                                                                           coords.x, coords.y, coords.z,
                                                                           resultsBase + val->equationIndex, eqSize);
                                rDone[val->equationIndex] = true;
                                --rDoneCount;
                            }

                            // XXX mode MethodPtr
//                            if (!(val->validator.*val->validator.check)(xx, yy, zz, results[val->equationIndex]))
                            if (!val->validator->Check(xx, yy, zz, results[val->equationIndex]))
                            {
                                check = false;
                                break;
                            }
                        }
                        if (check == true)
                        {
                            nuChunk->SetCube(x, y, z, (csi)->cubeType);
                            break;
                        }
                    }

                    results += eqSize;
                }
            }
        }

        Tools::DeleteTab(resultsBase);
        Tools::DeleteTab(rDone);

        callback(nuChunk);
    }

}}}}
