
enum class Enumeration {
    Val1,
    Val2,
    Val3 = 100,
    Val4 /* = 101 */
};

int main()
{
    Enumeration e = Enumeration::Val1;
}
