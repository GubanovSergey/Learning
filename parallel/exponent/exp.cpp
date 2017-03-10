#include "big_int.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <mpi.h>
#include <cstdlib>

/*bool isSender (int rank, int level) {
    return ((rank - (1 << (level - 1) - 1)) & (1 << level - 1)) == 0;
}

bool isRecver (int rank, int level) {
    int tmp = (1 << level - 1);
    return ((rank - tmp) & tmp) == 0;
}*/

void send_BigUint(BigUint & which, int to, std::ofstream & dump) {
    auto send_buf = std::move(which.cp_data());
    int nblocks = send_buf.size();
    //SEND denom
    //MPI_Send (buf, size, MPI_CHAR, dst, 0, MPI_COMM_WORLD)
    double timer = MPI_Wtime();
    MPI_Send(&nblocks, 1, MPI_INT, to, 0, MPI_COMM_WORLD);
    //f_dump << "[DBG] Num blocks to send " << nblocks << std::endl;
    MPI_Send(&send_buf.front(), nblocks, MPI_UNSIGNED_LONG_LONG, to, 0, MPI_COMM_WORLD);
    dump << "Sent with " << MPI_Wtime() - timer << "delay" << std::endl;
}

BigUint recv_BigUint(int from, MPI_Status * s, std::ofstream & dump) {
    double timer = MPI_Wtime();
    int nblocks;
    MPI_Recv(&nblocks, 1, MPI_INT, from, 0, MPI_COMM_WORLD, s);
    //f_dump << "[DBG] Num blocks to receive " << nblocks << std::endl;

    std::vector<BigUint::DataType> recv_buf(nblocks);
    assert(sizeof(BigUint::DataType) == sizeof(MPI_UNSIGNED_LONG_LONG));
    //f_dump << "[DBG] Container resized to " << denom_mult_buf.capacity() << " elements" << std::endl;
    MPI_Recv(&recv_buf.front(), nblocks, MPI_UNSIGNED_LONG_LONG, from, 0, MPI_COMM_WORLD, s);
    dump << "Got with delay " << MPI_Wtime() - timer << std::endl;
    return BigUint(std::move(recv_buf));
}


int main(int argc, char * argv[]) {
    unsigned prec, n_digits;
    int rank;

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
    MPI_Comm_size (MPI_COMM_WORLD, &proc_amnt);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    std::ofstream f_dump("dump_" + std::to_string(rank) + ".txt");
    std::ofstream f_ans("exp.txt");

    double local_timer = MPI_Wtime();
    int my_n = (rank == proc_amnt - 1) ? prec : (prec / proc_amnt * (rank + 1));
    int prev_n = prec / proc_amnt * rank;

    f_dump << "My_n is " << my_n << std::endl;

    BigUint sum(1), cur_addent(1);
    for (int cur_mult = my_n; cur_mult > prev_n + 1; cur_mult--) {
        cur_addent *= cur_mult;
        sum += cur_addent;
    }
    f_dump << "Sum counted in " << MPI_Wtime() - local_timer << std::endl;

    BigUint denom(cur_addent);

    MPI_Status stat;
    if (rank > 0) {
        double mult_timer = MPI_Wtime();
        denom *= (prev_n + 1);
        f_dump << "Wait for receive " << " " << MPI_Wtime() - local_timer << std::endl;

        BigUint denom_mult = std::move(recv_BigUint(rank - 1, &stat, f_dump));
        //f_dump << "[DBG] Multiplier " << denom_mult << std::endl;
        denom *= denom_mult;
        f_dump << my_n << "! is " << denom << std::endl;
        f_dump << "LONG mult time " << MPI_Wtime() - mult_timer << std::endl;
    }
    if (rank + 1 < proc_amnt) {
        f_dump << "Wait to send " << MPI_Wtime() - local_timer << std::endl;
        send_BigUint(denom, rank + 1, f_dump);
    }
    f_dump << "[DBG] Sum " << sum << std::endl;
    BigUint local_res = std::move(sum);
    local_res.mult10n(n_digits + proc_amnt / 2);
    f_dump << "[DBG] Shifted sum " << local_res << std::endl;

    f_dump << "Division!" << std::endl;
    local_res /= denom;
//TODO send a message if res = 0 meaning that their addents don't have to be counted
    std::cout << "Time " << rank << " " << MPI_Wtime() - local_timer << std::endl;
    f_dump << "Time " << MPI_Wtime() - local_timer << std::endl;

    auto l_res = local_res.separate(n_digits + proc_amnt / 2);
    f_dump << "Local ans = " << l_res[0] << "." << l_res[1] <<
    "\n[NOTE]Fraction part may have leading nils" << std::endl;
    if (rank > 0) {
        send_BigUint(local_res, 0, f_dump);
    }
    else {
        BigUint global_sum = std::move(local_res);
        for (int i = 1; i < proc_amnt; i++) {
            global_sum += recv_BigUint(i, &stat, f_dump);
        }
        auto ans = global_sum.separate(n_digits + proc_amnt / 2);
        ans[0] += 1;

        f_dump << "Answer = " << ans[0] << "." << ans[1] << std::endl;
        f_ans << "Answer = " << ans[0] << "." << ans[1] << std::endl;
    }


    /*//f_ans << "Ans = " << ans[0] << "." << ans[1] << "\nTime " << MPI_Wtime() - timer_init << "\n" << std::endl;
    std::cout << "Ans = " << ans[0] << "." << ans[1] << "\nTime " << MPI_Wtime() - timer_init << "\n" << std::endl;*/

    MPI_Finalize();
    return 0;
}
