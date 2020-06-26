#include "spch.h"

#include "GeneralTest.h"

struct HasEqual {
    bool operator==(const HasEqual& rhs) {
        return false;
    }
};

struct HasFormat {
    
};

inline std::ostream& operator<<(std::ostream& os, const HasFormat& e) {
		return os << " ";
}

struct HasEqualAndFormat {
    bool operator==(const HasEqualAndFormat& rhs) {
        return false;
    }
};

inline std::ostream& operator<<(std::ostream& os, const HasEqualAndFormat& e) {
	return os << " ";
}

void TestOperatorChecks() {
    WZ_INFO("========================================\n============== Operators ===============\n========================================");
    #define CHECK_OPS(c)\
    WZ_DEBUG("{0}\nself comparable : {1}\nhas fmt : {2}", #c,\
            wz::has_equality<c, c>(),\
            wz::has_leftshift_ostream<c>)

    CHECK_OPS(HasEqual);
    CHECK_OPS(HasFormat);
    CHECK_OPS(HasEqualAndFormat);
}

void TestMathFormatting() {
    WZ_INFO("========================================\n============== Math Formatting ===============\n========================================");
    wz::fvec2 fvec2(1, 2);
    wz::fvec3 fvec3(1, 2, 3);
    wz::fvec4 fvec4(1, 2, 3, 4);

    wz::ivec2 ivec2(1, 2);
    wz::ivec3 ivec3(1, 2, 3);
    wz::ivec4 ivec4(1, 2, 3, 4);

    wz::fmat4 fmat4(1);
    wz::imat4 imat4(1);

    WZ_DEBUG("fvec2: {}\nfvec3: {}\nfvec4: {}\nivec2: {}\nivec3: {}\nivec3: {}\n"
             "fmat4: {}\n imat4: {}\n",
             fvec2, fvec3, fvec4, ivec2, ivec3, ivec4, fmat4, imat4);
}

void GeneralTest::Init() {
    wz::Log::SetCoreLogLevel(LOG_LEVEL_DEBUG);
    TestOperatorChecks();
    TestMathFormatting();
}
void GeneralTest::LoadResources() {
    
}
void GeneralTest::Shutdown() {

}