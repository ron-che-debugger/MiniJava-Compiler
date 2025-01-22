# Ensure spim.linux is executable
chmod +x ./spim.linux

# Function to run the first loop with `codegen.linux`
run_codegen() {
    for i in $(seq 1 10); do
        ./codegen.linux < ./test/src$i > ast_symbol_table_groundtruth$i.txt
        echo 1 | ./spim.linux -quiet -file code.s > codegen_groundtruth$i.out
        cp code.s codegen_groundtruth$i.s
        dos2unix codegen_groundtruth$i.out
    done
}

# Function to run the second loop with `codegen` and generate ast_symbol_table_i.txt
compare_outputs() {
    for i in $(seq 1 10); do
        LD_LIBRARY_PATH=. ./codegen < ./test/src$i > ast_symbol_table_$i.txt
        echo 1 | ./spim.linux -quiet -file code.s > codegen$i.out
        cp code.s codegen_$i.s
        dos2unix codegen$i.out

        if diff -b codegen_groundtruth$i.out codegen$i.out > /dev/null; then
            echo "[PASS] Output for src$i matches expected results."
        else
            echo "[FAIL] Output for src$i does not match expected results."
        fi
    done
}

# Main script execution
run_codegen
compare_outputs