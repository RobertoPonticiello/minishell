#!/bin/bash

# Colori per output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# Funzione per testare un comando
test_command() {
    echo -e "\nTest: $1"
    echo "Comando: $2"
    echo "Output atteso: $3"
    echo "Output reale:"
    ./minishell << EOF
$2
exit
EOF
    echo "----------------------------------------"
}

# Test virgolette singole
test_command "Virgolette singole semplici" "echo 'ciao mondo'" "ciao mondo"
test_command "Virgolette singole con virgolette doppie" "echo 'ciao \"mondo\"'" "ciao \"mondo\""
test_command "Virgolette singole con variabile" "echo 'ciao $USER'" "ciao $USER"

# Test virgolette doppie
test_command "Virgolette doppie semplici" "echo \"ciao mondo\"" "ciao mondo"
test_command "Virgolette doppie con virgolette singole" "echo \"ciao 'mondo'\"" "ciao 'mondo'"
test_command "Virgolette doppie con variabile" "echo \"ciao $USER\"" "ciao $USER"

# Test escape
test_command "Escape newline" "echo \"ciao\nmondo\"" "ciao\nmondo"
test_command "Escape virgolette" "echo \"ciao \\\"mondo\\\"\"" "ciao \"mondo\""
test_command "Escape dollaro" "echo \"ciao \\$USER\"" "ciao $USER"

# Test variabili d'ambiente
test_command "Variabile semplice" "echo $USER" "$USER"
test_command "Variabile tra virgolette" "echo \"$USER\"" "$USER"
test_command "Variabile non esistente" "echo $NONEXISTENT" ""
test_command "Stato di uscita" "echo $?" "0"

# Test combinati
test_command "Combinazione virgolette" "echo \"ciao 'mondo' e $USER\"" "ciao 'mondo' e $USER"
test_command "Combinazione escape" "echo \"ciao \\\"mondo\\\" e \\$USER\"" "ciao \"mondo\" e $USER"
test_command "Combinazione completa" "echo \"ciao 'mondo' e \\\"$USER\\\"\"" "ciao 'mondo' e \"$USER\""

echo -e "\nTest completati!" 