#include "server2/game/map/gen/ChunkGenerator.hpp"
#include "server2/game/map/gen/CubeSpawnInfo.hpp"
#include "server2/game/map/gen/Perlin.hpp"
#include "server2/game/map/gen/RandMersenneTwister.hpp"

#include "common/CubeType.hpp"

#include "server2/Logger.hpp"

#include "server2/game/map/Conf.hpp"

#include "server2/game/map/gen/equations/Equations.hpp"
#include "server2/game/map/gen/validators/Validators.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen {

    ChunkGenerator::ChunkGenerator(Conf const& conf) :
        Tools::SimpleMessageQueue(boost::thread::hardware_concurrency() + 1)
    {
        Tools::debug << "ChunkGenerator::ChunkGenerator()\n";
        Log::load << "Loading chunk generator:\n" <<
                     "name: " << conf.name << "\n" <<
                     "fullname: " << conf.fullname << "\n" <<
                     "is_default: " << conf.is_default << "\n";

        std::map<std::string, Chunk::CubeType> typeIds;

        for (auto it = conf.cubes.begin(), ite = conf.cubes.end() ; it != ite ; ++it)
        {
            std::cout << it->second.type->name << ": " << it->second.type->id << "\n";

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
            std::list<CubeSpawnInfo::Validation> validations;
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
                this->_cubes.push_back(new CubeSpawnInfo((*vbit)->cube_type->id, validations));
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
    }

    ChunkGenerator::~ChunkGenerator()
    {
        Tools::debug << "ChunkGenerator::~ChunkGenerator()\n";

        for (auto it = this->_cubes.begin(), ite = this->_cubes.end() ; it != ite ; ++it)
            Tools::Delete(*it);
        for (auto it = this->_equations.begin(), ite = this->_equations.end() ; it != ite ; ++it)
            Tools::Delete(*it);
        Tools::Delete(this->_perlin);
    }

    void ChunkGenerator::Start()
    {
        Tools::debug << "ChunkGenerator::Start()\n";
        this->_Start();
    }

    void ChunkGenerator::Stop()
    {
        Tools::debug << "ChunkGenerator::Stop()\n";
        this->_Stop();
    }

    void ChunkGenerator::_GetChunk(Chunk::IdType id, Callback callback)
    {
        Chunk::CoordsType coords = Chunk::IdToCoords(id);
        Uint32 x, y, z;
        double xx, yy, zz;
        Chunk* nuChunk = new Chunk(coords);

        double* results = new double [this->_equations.size()];
        bool* results_done = new bool [this->_equations.size()];
        for (unsigned int i = 0 ; i < this->_equations.size() ; ++i)
            results_done[i] = false;

        for (x = 0 ; x < Common::ChunkSize ; ++x)
        {
            xx = (double)((int)coords.x - (1 << 21)) + (double)x / Common::ChunkSize;
            for (z = 0 ; z < Common::ChunkSize ; ++z)
            {
                zz = (double)((int)coords.z - (1 << 21)) + (double)z / Common::ChunkSize;

                for (y = 0 ; y < Common::ChunkSize ; ++y)
                {
                    yy = (double)((int)coords.y - (1 << 19)) + (double)y / Common::ChunkSize;

                    for (auto it = this->_cubes.begin(),
                            ite = this->_cubes.end() ; it != ite ; ++it)
                    {

                        auto const& validations = (*it)->GetValidations();
                        bool check = true;
                        for (auto vit = validations.begin(), vite = validations.end(); vit != vite; ++vit)
                        {
                            if (!results_done[vit->equationIndex])
                            {
                                results_done[vit->equationIndex] = true;
                                results[vit->equationIndex] = this->_equations[vit->equationIndex]->Calc(xx, yy, zz);
                            }
                            if (!vit->validator->Check(xx, yy, zz, results[vit->equationIndex]))
                            {
                                check = false;
                                break;
                            }
                        }
                        if (check == true)
                        {
                            nuChunk->SetCube(x, y, z, (*it)->cubeType);
                            break;
                        }
                    }

                    // reinit tout si il faut, sinon juste les 3D
                    for (unsigned int i = 0 ; i < this->_equations.size() ; ++i)
                    {
                        if (y == Common::ChunkSize - 1 || !this->_equations[i]->Is2D())
                            results_done[i] = false;
                    }
                }
            }
        }

        Tools::DeleteTab(results_done);
        Tools::DeleteTab(results);

        callback(nuChunk);
    }

}}}}
