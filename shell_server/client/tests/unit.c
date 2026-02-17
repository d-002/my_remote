#include <criterion/criterion.h>
#include <criterion/new/assert.h>

TestSuite(Dummy);

Test(Dummy, DummyTest) {
    cr_expect(1);
}
