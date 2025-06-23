#!/bin/bash

MINISHELL=./minishell
TMP_OUT=tmp_minishell_out.txt
TMP_EXP=tmp_minishell_exp.txt

pass() { echo -e "\033[32m[PASS]\033[0m $1"; }
fail() { echo -e "\033[31m[FAIL]\033[0m $1"; }

run_test() {
    desc="$1"
    cmd="$2"
    expected="$3"

    echo -e "$cmd" | $MINISHELL 2>&1 | grep -v "^minishell\\$" > $TMP_OUT
    echo -e "$expected" > $TMP_EXP

    if diff -q $TMP_OUT $TMP_EXP > /dev/null; then
        pass "$desc"
    else
        fail "$desc"
        echo "  Output ottenuto:"
        cat $TMP_OUT
        echo "  Output atteso:"
        cat $TMP_EXP
    fi
    rm -f $TMP_OUT $TMP_EXP
}

# 1. Builtin echo
run_test "echo" "echo Hello" "Hello"

# 2. Builtin pwd
run_test "pwd" "pwd" "$(pwd)"

# 3. Builtin cd
run_test "cd" "cd ..\npwd" "$(dirname "$(pwd)")"

# 4. Builtin export/env
run_test "export/env" "export TESTVAR=ciao\necho \$TESTVAR" "ciao"

# 5. Builtin unset
run_test "unset" "export REMOVE=delete\nunset REMOVE\necho \$REMOVE" ""

# 6. Builtin exit (non confrontabile, solo verifica che esca)
echo -e "exit" | $MINISHELL > /dev/null 2>&1 && fail "exit" || pass "exit"

# 7. Comando vuoto
run_test "comando vuoto" "" ""

# 8. Solo spazi
run_test "solo spazi" "     " ""

# 9. Solo tabs
run_test "solo tabs" "		" ""

# 10. Single quotes (no espansione)
run_test "single quotes" "echo 'Il mio \$HOME'" "Il mio \$HOME"

# 11. Double quotes (espansione)
run_test "double quotes" "export VAR=ciao\necho \"Il mio \$VAR\"" "Il mio ciao"

# 12. Pipe semplice
run_test "pipe semplice" "echo uno due tre | wc -w" "3"

# 13. Pipe multipla
run_test "pipe multipla" "echo -e \"a\nb\nc\" | grep b | wc -l" "1"

# 14. Redirezione output >
run_test "redirezione output" "echo ciao > testfile.txt\ncat testfile.txt" "ciao"
rm -f testfile.txt

# 15. Redirezione input <
echo "inputfile" > input.txt
run_test "redirezione input" "cat < input.txt" "inputfile"
rm -f input.txt

# 16. Redirezione append >>
run_test "redirezione append" "echo prima > append.txt\necho seconda >> append.txt\ncat append.txt" "prima\nseconda"
rm -f append.txt

# 17. Pipe + redirezione
run_test "pipe + redirezione" "echo -e \"uno\ndue\ntre\" | grep due > out.txt\ncat out.txt" "due"
rm -f out.txt

echo "Tutti i test completati."