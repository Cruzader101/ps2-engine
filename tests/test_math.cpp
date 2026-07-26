#include <cstdio>
#include <math/vec3.h>
#include <math/Mat4.h>

static int g_failures = 0;

#define CHECK(cond) \
  do { \
    if (!(cond)) { \
       printf("FAILED: %s\n  at %s:%d\n", \
       #cond, __FILE__, __LINE__); \
       ++g_failures; \
    } \
  } while (0)

static void test_mat4_identity() {
   Mat4 i_1 = Mat4::identity();
   Mat4 i_2 = Mat4::identity();
   Mat4 m_1{1, 2, 1, 3, 1.04f, 1.23f, 1.44f, 1.56f, 1.01f, 1.59f, 1, 1, 1, 1, 2.00f, 1.87f,};

   Mat4 m_check1 = i_1 * m_1;
   Mat4 m_check2 = m_1 * i_1;

   Mat4 i_check = i_1 * i_2;

   CHECK((approx_equal(m_1, m_check1)));
   CHECK((approx_equal(m_1, m_check2)));
   CHECK((approx_equal(i_1, i_check)));

}

static void test_mat4_translate_indices() {

}

static void test_mat4_translate_compose() {

}

int main () {
    test_mat4_identity();

    if (g_failures == 0) {
        printf("all math tests passed\n");
        return 0;
    }
    printf("%d math test(s) FAILED\n", g_failures);
    return 1;
}
