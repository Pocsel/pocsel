
#include <iostream>
#include <vector>
#include <cstring>

#include "common/Packet.hpp"
#include "Assert.hpp"

int main(int, char**)
{
    Packet p;
    p.Write8(0x11);
    p.Write16(0x2233);
    p.Write32(0x44556677);
    p.Write64(0x8899001122334455);
    p.Write64(-9223371487090573312LL);
    p.WriteFloat(3.145f);
    p.WriteString("Test string BITE\n\t\n");
    std::vector<char> vec(8);
    vec[0] = 0x11;
    vec[1] = 0x22;
    vec[2] = 0x33;
    vec[3] = 0x44;
    vec[4] = 0x55;
    vec[5] = 0x66;
    vec[6] = 0x77;
    vec[7] = 0x88;
    p.WriteData(vec);
    char const* data = p.GetCompleteData();

    Packet pr;
    pr.SetData(data + 2, p.GetSize());
    Assert(pr.Read8() == 0x11);
    Assert(pr.Read16() == 0x2233);
    Assert(pr.Read32() == 0x44556677);
    Assert(pr.Read64() == 0x8899001122334455);
    Assert(pr.Read64() == -9223371487090573312LL);
    Assert(pr.ReadFloat() == 3.145f);
    Assert(pr.ReadString() == "Test string BITE\n\t\n");
    std::vector<char> readVec = pr.ReadData();
    Assert(readVec.size() == vec.size());
    Assert(std::memcmp(readVec.data(), vec.data(), vec.size()) == 0);

    return 0;
}
