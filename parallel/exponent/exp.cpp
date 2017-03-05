#include "big_int.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <mpi.h>
#include <cstdlib>

int main(int argc, char * argv[]) {
    BigUint cur_addent(1);
    unsigned prec, n_digits;

    int init = MPI_Init (&argc, &argv);
    if (init != MPI_SUCCESS) {
        printf("MPI Initialization failed! \n");
        MPI_Abort(MPI_COMM_WORLD, init);
    }

    if (argc < 3) {
        printf("Usage ./exp <last_elem> <precision>\n");
        return 0;
    }
    prec = std::atoi (argv[1]);
    n_digits = std::atoi (argv[2]);

    int proc_amnt;
    std::cout << "Input num_proc: ";
    std::cin >> proc_amnt;

    std::ofstream f_ans("exp.txt");
    double timer_init = MPI_Wtime();
    BigUint sh_result(0);
    BigUint cur_denom(1);

    for (int my_num = 0; my_num < proc_amnt; my_num ++) {   //TODO make load of processes almost equal
        double local_timer = MPI_Wtime();
        int my_n = (my_num == proc_amnt - 1) ? prec : (prec / proc_amnt * (my_num + 1));
        int prev_n = prec / proc_amnt * my_num;

        std::cout << "My_n is " << my_n << std::endl;

        BigUint sum(1), cur_addent(1);
        for (int cur_mult = my_n; cur_mult > prev_n + 1; cur_mult--) {
            cur_addent *= cur_mult;
            sum += cur_addent;
        }
        std::cout << "Sum counted in " << MPI_Wtime() - local_timer << std::endl;

        BigUint denom_mult = cur_addent;
        if (cur_denom == 1) {
            cur_denom = denom_mult;
        }
        else {
            double mult_timer = MPI_Wtime();
            cur_denom *= (prev_n + 1);
            cur_denom *= denom_mult;
            std::cout << "LONG mult time " << MPI_Wtime() - mult_timer << std::endl;
        }

        BigUint local_res = sum;
        local_res.mult10n(n_digits + proc_amnt / 2);

        std::cout << "Division!" << std::endl;
        local_res /= cur_denom;

        sh_result += local_res;
        std::cout << "Time " << my_num << " " << MPI_Wtime() - local_timer << std::endl;
    }
    //TODO round result
    auto ans = sh_result.separate(n_digits + proc_amnt / 2);
    ans[0] += 1;

    f_ans << "Ans = " << ans[0] << "." << ans[1] << "\nTime " << MPI_Wtime() - timer_init << "\n" << std::endl;
    std::cout << "Time " << MPI_Wtime() - timer_init << std::endl;

    MPI_Finalize();
    return 0;
}
