
#include <structures/vector.h>

#include "test.h"

int main(int argc, char **argv)
{
    BEGIN_TESTS();

    {
        struct bsal_vector vector;
        int i;
        int value;
        int *pointer;
        int expected;

        bsal_vector_init(&vector, sizeof(int));
        TEST_INT_EQUALS(bsal_vector_size(&vector), 0);

        for (i = 0; i < 1000; i++) {
            TEST_POINTER_EQUALS(bsal_vector_at(&vector, i), NULL);
            bsal_vector_push_back(&vector, &i);
            TEST_INT_EQUALS(bsal_vector_size(&vector), i + 1);

            value = *(int *)bsal_vector_at(&vector, i);

            TEST_POINTER_NOT_EQUALS(bsal_vector_at(&vector, i), NULL);
            TEST_INT_EQUALS(value, i);

            pointer = (int *)bsal_vector_at(&vector, i);
            expected = i * i * i;
            *pointer = expected;

            value = *(int *)bsal_vector_at(&vector, i);

            TEST_INT_EQUALS(value, expected);
        }

        bsal_vector_resize(&vector, 42);
        TEST_INT_EQUALS(bsal_vector_size(&vector), 42);

        TEST_POINTER_EQUALS(bsal_vector_at(&vector, 42), NULL);
        TEST_POINTER_EQUALS(bsal_vector_at(&vector, 43), NULL);
        TEST_POINTER_NOT_EQUALS(bsal_vector_at(&vector, 41), NULL);
        TEST_POINTER_NOT_EQUALS(bsal_vector_at(&vector, 0), NULL);
        TEST_POINTER_NOT_EQUALS(bsal_vector_at(&vector, 20), NULL);

        bsal_vector_resize(&vector, 100000);
        TEST_INT_EQUALS(bsal_vector_size(&vector), 100000);
        TEST_POINTER_NOT_EQUALS(bsal_vector_at(&vector, 100000 - 1), NULL);
        TEST_POINTER_NOT_EQUALS(bsal_vector_at(&vector, 0), NULL);
        TEST_POINTER_EQUALS(bsal_vector_at(&vector, 2000000), NULL);

        bsal_vector_destroy(&vector);
    }

    END_TESTS();

    return 0;
}
