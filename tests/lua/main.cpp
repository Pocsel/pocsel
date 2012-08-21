#include "tools/precompiled.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"
#include "tools/lua/Iterator.hpp"
#include "tools/lua/MetaTable.hpp"
#include "tools/lua/utils/Utils.hpp"
#include "tools/lua/WeakResourceRefManager.hpp"
#include "tools/lua/AWeakResourceRef.hpp"
#include "tools/Timer.hpp"

#define STRINGIFY(...) #__VA_ARGS__

void f(Tools::Lua::CallHelper& call)
{
    auto const& args = call.GetArgList();
    auto it = args.begin();
    auto itEnd = args.end();
    for (; it != itEnd; ++it)
        Tools::log << "arg: " << it->ToNumber() << std::endl;
    Tools::log << call.PopArg().CheckBoolean() << std::endl;
    Tools::log << call.PopArg().CheckString() << std::endl;
    Tools::log << "f()" << std::endl;
    call.PushRet(call.GetInterpreter().MakeNumber(1.5));
    call.PushRet(call.GetInterpreter().MakeNumber(2.6));
}

using namespace Tools::Lua;

void TestFunction(int test)
{
    std::cout << test << std::endl;
}

static void Basic(Interpreter& i)
{
    i.DoString("test = {} test[4] = 23.34");

    Tools::Lua::Ref bite = i.Bind(&f, std::placeholders::_1);
    //Tools::Lua::Ref bite = i.MakeFunction(std::bind(&f, std::placeholders::_1));

    try
    {
        Tools::log << bite(1, 2).ToNumber() << " jksdfgjk " << sizeof(Tools::Lua::Ref) << std::endl;
    }
    catch (std::exception& e)
    {
        Tools::log << "from c++: " << e.what() << std::endl;
    }

    i.Globals().Set(std::string("bite"), bite);

    try
    {
        i.DoString("bite(1, 2)");
    }
    catch (std::exception& e)
    {
        Tools::log << "from lua: " << e.what() << std::endl;
    }

    {
        Tools::Lua::CallHelper helper(i);
        helper.PushArg(i.Make(true));
        helper.PushArg(i.Make("Hey!!"));
        bite.Call(helper);
        auto const& rets = helper.GetRetList();
        auto it = rets.begin();
        auto itEnd = rets.end();
        for (; it != itEnd; ++it)
            Tools::log << "ret: " << it->ToString() << std::endl;
        i.DoString("ret1, ret2 = bite(false, \"ZOB\")");
        Tools::log << "ret1: " << i.Globals()["ret1"].ToString() << ", ret2: " << i.Globals()["ret2"].ToString() << std::endl;
    }
}

static void Iterators(Interpreter& i)
{
    i.DoString("test = \"sjkldg\\nlkéhsdfg\" g = 323.4 jdsk = {} iwer = {} jkd = function() end");

    Tools::Lua::Iterator it = i.Globals().Begin();
    Tools::Lua::Iterator itEnd = i.Globals().End();
    for (; it != itEnd; ++it)
    {
        Tools::log << it.GetValue().GetTypeName() << ": " << it.GetValue().ToString() << " (key " << it.GetKey().GetTypeName() << " " << it.GetKey().ToString() << ")" << std::endl;
    }
}

static void MetaTables(Interpreter& i)
{
    i.DoString("FUCK = function() print \"FUCK\" end");
    Tools::Lua::Ref meta = i.MakeTable();
    meta.Set("__index", i.Globals()["FUCK"]);
    meta.Set(1, "test");
    meta.Set(2, "tests2");
    Tools::log << "Type of FUCK: " << meta["__index"].GetTypeName() << std::endl;
    Tools::log << "size of metatable: " << meta.GetLength() << std::endl;
    Tools::Lua::Ref metaTest = i.MakeTable();
    metaTest.SetMetaTable(meta);
    i.Globals().Set("metaTest", metaTest);
    i.DoString("a = metaTest[12]");
    i.DoString("b = metaTest[14]");
    i.DoString("c = metaTest[15]");
}

static void Serialization(Interpreter& i)
{
    i.DoString("sharedTable = { 213, 23, \"sdfg\" }");
    i.DoString("serializeTest = { bite = { 1, 3, 4, sharedTable }, test = \"HEY\", allo = false }");
    std::string serialized = i.GetSerializer().Serialize(i.Globals()["serializeTest"]);
    Tools::log << "serialize1: \"\"\"" << serialized << "\"\"\"\n";
    try
    {
        Tools::log << "serialize2: \"\"\"" << i.GetSerializer().Serialize(i.Globals()["FUCK"]) << "\"\"\"\n";
    }
    catch (std::exception& e)
    {
        Tools::log << "serialize fail: " << e.what() << std::endl;
    }
    Tools::log << "serialize3: \"\"\"" << i.GetSerializer().Serialize(i.Globals()["test"]) << "\"\"\"\n";
    Tools::log << "Deserialize: \"\"\"" << i.GetSerializer().Serialize(i.GetSerializer().Deserialize(serialized)) << "\"\"\"\n";

    Tools::log << "------- Begin super test de serialization -------" << std::endl;
    auto testObj = i.MakeTable();
    auto testObjMetaTable = i.MakeTable();
    i.DoString("testObjSerialize = function(obj) return \"return nil\" end -- lol");
    testObjMetaTable.Set("__serialize", i.Globals()["testObjSerialize"]);
    testObj.SetMetaTable(testObjMetaTable);
    auto t = i.MakeTable();
    t.Set("h", i.MakeFunction(std::bind(&f, std::placeholders::_1)));
    t.Set("obj", testObj);
    try
    {
        Tools::log << i.GetSerializer().Serialize(t, true) << std::endl;
    }
    catch (std::exception& e)
    {
        Tools::log << "error: " << e.what() << std::endl;
    }
}

static void MetaTableCpp(Interpreter& i)
{
    struct A
    {
        void Print(int /* nb */) { Tools::log << "A::Print()\n"; }
    };

    MetaTable::Create(i, A());

    try
    {
        i.DoString("table = getmetatable(m)");
    }
    catch (std::exception& e)
    {
        Tools::log << "getmetatable(UserData): " << e.what() << "\n";
    }

    i.Globals().Set("TestFunction", i.Bind(&TestFunction, std::placeholders::_1));
    i.DoString("TestFunction(10)");
    auto r = i.Globals().Set("A", i.Bind(&A::Print, std::placeholders::_1, std::placeholders::_2));
    r(A(), 50);

    //// glm::dvec2 - MetaTable
    struct fakeVec2
    {
        double x, y;
        fakeVec2(double x = 0, double y = 0) : x(x), y(y) {}
        fakeVec2(glm::dvec2 const& v) : x(v.x), y(v.y) {}
        static fakeVec2 Normalize(fakeVec2 const& v) { return glm::normalize(glm::dvec2(v.x, v.y)); }
        static double Dot(fakeVec2 const& v1, fakeVec2 const& v2) { return glm::dot(glm::dvec2(v1.x, v1.y), glm::dvec2(v2.x, v2.y)); }
    };
    auto& mVec2 = MetaTable::Create(i, fakeVec2());
    mVec2.SetMethod("Normalize", i.Bind(static_cast<fakeVec2(*)(fakeVec2 const&)>(&fakeVec2::Normalize), std::placeholders::_1));
    mVec2.SetMethod("Dot", i.Bind(static_cast<double(*)(fakeVec2 const&, fakeVec2 const&)>(&fakeVec2::Dot), std::placeholders::_1, std::placeholders::_2));
    mVec2.SetMetaMethod(MetaTable::Serialize,
        [&i](CallHelper& helper)
        {
            auto v = helper.PopArg().Check<fakeVec2*>();
            helper.PushRet(i.MakeString("return Vec2.New(" + Tools::ToString(v->x) + ", " + Tools::ToString(v->y) + ")"));
        });
    mVec2.SetMethod("Dump",
        [](CallHelper& helper)
        {
            auto v = helper.PopArg().Check<fakeVec2*>();
            Tools::log << "(" << v->x << ";" << v->y << ")\n";
        });
    // Namespace
    auto n = i.Globals().GetTable("Vec2");
    n.Set("New", i.MakeFunction(
        [&i](CallHelper& helper)
        {
            auto x = helper.PopArg().Check<double>();
            auto y = helper.PopArg().Check<double>();
            helper.PushRet(i.Make(fakeVec2(x, y)));
        }));
    i.DoString(STRINGIFY(
        v = Vec2.New(2, 3)
        v:Dump()
        v:Normalize()
        v:Dump()
        m = "m"
        m1 = "m1"
        m2 = "m2"

        tab = {
            [v] = m,
            [2] = m1,
            ['toto'] = m2
        }
    ));

    Tools::log << "first: " << i.GetSerializer().Serialize(i.Globals()["tab"]) << std::endl;
    Tools::log << "second: " << i.GetSerializer().Serialize(i.GetSerializer().Deserialize(i.GetSerializer().Serialize(i.Globals()["tab"]))) << std::endl;
}

static void Benchmark(Interpreter& i)
{
    Tools::Timer timer;
    Tools::log << "Benchmark vector 3\n";
    i.DoString("a = Utils.Vector3(1.5, 2.5, 3.5)");
    i.DoString("b = Utils.Vector3(-1.5, -2.5, -3.5)");
    i.DoString("for i=1,5000 do c = a + b end");
    Tools::log << "End (time: " << timer.GetPreciseElapsedTime() << " us)\n";

    timer.Reset();
    Tools::log << "Benchmark matrix 4\n";
    i.DoString("m = Utils.Matrix4()");
    i.DoString("m2 = Utils.Matrix4()");
    i.DoString("for i=1,5000 do m3 = m * m2 end");
    for (int _ = 0; _ < 2500; ++_)
    {
        auto matrix = i.Globals()["m"];
        //i.Globals()["m"].Check<glm::mat4*>();
        if (matrix.IsTable())
        {
            glm::mat4 m(
                matrix["m00"].To<float>(), matrix["m01"].To<float>(), matrix["m02"].To<float>(), matrix["m03"].To<float>(),
                matrix["m10"].To<float>(), matrix["m11"].To<float>(), matrix["m12"].To<float>(), matrix["m13"].To<float>(),
                matrix["m20"].To<float>(), matrix["m21"].To<float>(), matrix["m22"].To<float>(), matrix["m23"].To<float>(),
                matrix["m30"].To<float>(), matrix["m31"].To<float>(), matrix["m32"].To<float>(), matrix["m33"].To<float>());
        }
    }
    Tools::log << "End (time: " << timer.GetPreciseElapsedTime() << " us)\n";
}

static void RegisteredTypes(Interpreter& i)
{
    i.DoString(STRINGIFY(
        v = Utils.Vector4(1.5, 2.25, 3.125, 4.06125)
        m = Utils.Matrix4(2)
        m.m01 = 01
        m.m10 = 10
        m = m*m
        print("print(v): (v is a Vector4)")
        print(v)
        print("print(m): (m is a Matrix4)")
        print(m)
        ));

    std::cout << "C++ v.ToString(): \"" << i.Globals()["v"].ToString() << "\"\n";

    std::cout << "C++ m*m:\n";
    glm::mat4 m(2);
    m[0][1] = 1;
    m[1][0] = 10;
    m = m*m;
    for (int i = 0; i < 4; ++i)
        std::cout << "(" << m[i][0] << " " << m[i][1] << " " << m[i][2] << " " << m[i][3] << ")\n";
}

static void Resources(Interpreter& i)
{
    struct MitoResourceManager
    {
        MitoResourceManager(Interpreter& i) : coucou(1336), coucou2(i.MakeTable()) {}
        int coucou;
        Ref coucou2;
    };

    struct ResourceDeTest : Tools::Lua::AWeakResourceRef<MitoResourceManager>
    {
        ResourceDeTest() : field1(0) {}
        ResourceDeTest(int field1, std::string field2) : field1(field1), field2(field2) {}
        virtual bool IsValid(MitoResourceManager const&) const { return this->field1 && !this->field2.empty(); }
        virtual void Invalidate(MitoResourceManager const&) { this->field1 = 0; this->field2.clear(); }
        virtual Ref GetReference(MitoResourceManager const& manager) const { return manager.coucou2; }
        bool operator <(ResourceDeTest const& rhs) const { return this->field1 < rhs.field1 && this->field2 < rhs.field2; }
        int field1;
        std::string field2;
    };

    MitoResourceManager realManager(i);
    WeakResourceRefManager<ResourceDeTest, MitoResourceManager> luaManager(i, realManager, true /* use fake references */);

    auto pair1 = luaManager.NewResource(ResourceDeTest(12, "resource1"));
    Uint32 resource1Id = pair1.first;
    Ref resource1 = pair1.second;
    auto pair2 = luaManager.NewResource(ResourceDeTest(24, "resource2"));
    //Uint32 resource2Id = pair2.first;
    Ref resource2 = pair2.second;
    i.Globals().Set("maResource1", resource1);
    i.Globals().Set("maResource2", resource2);
    i.DoString(STRINGIFY(
                print("--- test resources ---")
                print(" - avant invalidation")
                print("call 1 : ", tostring(maResource1:Lock()))
                print("call 2 : ", tostring(maResource2:Lock()))
                resource2FakeRef = maResource2:Lock()
                ));
    luaManager.InvalidateResource(resource1Id);
    luaManager.InvalidateAllFakeReferences();
    i.DoString(STRINGIFY(
                print(" - apres invalidation")
                print("call 1 : ", tostring(maResource1:Lock()))
                print("call 2 : ", tostring(maResource2:Lock()))
                resource2FakeRef = maResource2:Lock()
                resource2FakeRef.test = 12
                ));
    try
    {
        i.DoString(STRINGIFY(
                    resource1FakeRef = maResource1:Lock()
                    resource1FakeRef.test = 12
                    ));
    }
    catch (std::exception& e)
    {
        Tools::log << "This is expected: " << e.what() << std::endl;
    }
}

void ResourcesSerialization(Interpreter& i)
{
    class EntityManager
    {
        public:
            EntityManager(Interpreter& interpreter) : _interpreter(interpreter)
            {
                this->_entities.insert(std::make_pair(1, this->_interpreter.MakeNumber(66.5)));
                this->_entities.insert(std::make_pair(2, this->_interpreter.MakeString("hello")));
                this->_entities.insert(std::make_pair(3, this->_interpreter.MakeTable()));
                this->_entities.insert(std::make_pair(4, this->_interpreter.MakeBoolean(false)));
            }
            Ref GetEntity(Uint32 id) const
            {
                auto it = this->_entities.find(id);
                if (it == this->_entities.end())
                    throw std::runtime_error("Oops! This entity does not exist");
                return it->second;
            }
        private:
            Interpreter& _interpreter;
            std::map<Uint32, Ref> _entities;
    };

    struct EntityPtr : Tools::Lua::AWeakResourceRef<EntityManager>
    {
        EntityPtr(Uint32 id) : id(id) {}
        EntityPtr() : id(0) {}
        virtual bool IsValid(EntityManager const&) const { return this->id; }
        virtual void Invalidate(EntityManager const&) { this->id = 0; }
        virtual Ref GetReference(EntityManager const& manager) const { return manager.GetEntity(id); }
        bool operator <(EntityPtr const& rhs) const { return this->id < rhs.id; }
        void TryToLoad(EntityManager const& manager) { Tools::log << "EntityPtr::TryToLoad() - " << this->id << std::endl; }
        Uint32 id;
    };

    EntityManager entityManager(i);
    WeakResourceRefManager<EntityPtr, EntityManager> weakResourceRefManager(i, entityManager, true /* use fake references */);

    auto res = weakResourceRefManager.NewResource(EntityPtr(2));
    i.Globals().Set("unloadedResource1", weakResourceRefManager.NewUnloadedResource(EntityPtr(2)));
    i.Globals().Set("unloadedResource2", weakResourceRefManager.NewUnloadedResource(EntityPtr(5)));

    i.DoString(STRINGIFY(
                print("-------- Test Serialization Weak Ptr --------")
                print(type(unloadedResource1:Lock()))
                print(type(unloadedResource2:Lock()))
                print(type(unloadedResource1:Lock()))
                ));
}

int main(int, char**)
{
    {
        Tools::Lua::Interpreter i;
        i.RegisterLib(Tools::Lua::Interpreter::Base);
        i.RegisterLib(Tools::Lua::Interpreter::Math);
        Utils::RegisterMatrix(i);
        Utils::RegisterVector(i);

        // Tests basiques (assignation, ...)
        Basic(i);

        // Test itération des globales
        Iterators(i);

        // Test meta-tables lua
        MetaTables(i);

        // Test serialization simple (lua only)
        Serialization(i);

        // Test types C++ (avec serialization)
        MetaTableCpp(i);

        // Benchmark vector / matrix multiplication + récupération du résultat en C++
        Benchmark(i);

        // Test les types Vector et matrix
        RegisteredTypes(i);

        // Test ResourceManager
        Resources(i);

        // Test serialization resource pointers
        ResourcesSerialization(i);
    }


#ifdef _WIN32
    std::cin.get();
#endif
    return 0;
}
