#include "big_int.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <mpi.h>
#include <cstdlib>

/*int main(int argc, char * argv[]) {
  BigUint cur_addent(1), denom;
  unsigned short prec, cur_mult, n_digits;
  double last_time, new_time;

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

  last_time = MPI_Wtime();
  BigUint result(cur_addent);
  for (cur_mult = prec; cur_mult > 0; cur_mult--) {
    cur_addent *= cur_mult;
    result += cur_addent;
    //std::cout << result << "on iteration " << prec - cur_mult << std::endl;
  }
  denom = cur_addent;

  new_time = MPI_Wtime();

  std::cout << result << " / " << denom << "\nTime: " << new_time - last_time << std::endl;
  last_time = new_time;

  result.mult10n(n_digits);
  //std::cout << "Divident * 10^" << n_digits << " = " << result << std::endl;

  result /= denom;
  auto ans = result.separate(n_digits);

  new_time = MPI_Wtime();
  std::cout << ans[0] << "." << ans[1] << "\nTime: " << new_time-last_time << std::endl;

  MPI_Finalize();
  return 0;
}*/

int main(int argc, char * argv[]) {
    BigUint cur_addent(1);
    unsigned short prec, n_digits;

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
    for (int my_num = 0; my_num < proc_amnt; my_num ++) {
        double local_timer = MPI_Wtime();
        int my_n = (my_num == proc_amnt - 1) ? prec : (prec / proc_amnt * (my_num + 1));
        int prev_n = prec / proc_amnt * my_num;

        //std::cout << "My_n is " << my_n << std::endl;

        BigUint sum(1), cur_addent(1);
        for (int cur_mult = my_n; cur_mult > 1; cur_mult--) {
            cur_addent *= cur_mult;
            sum += cur_addent;
        }
        //std::cout << "Sum counted!" << std::endl;

        BigUint denom_mult = cur_addent;
        if (cur_denom == 1) {
            cur_denom = denom_mult;
        }
        else {
            cur_denom *= prev_n;
            cur_denom *= denom_mult;
        }

        BigUint local_res = sum;
        local_res.mult10n(n_digits + proc_amnt / 2);
        //std::cout << "Here!" << std::endl;

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
