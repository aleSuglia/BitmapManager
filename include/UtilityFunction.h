#ifndef UTILITYFUNCTION_H
#define UTILITYFUNCTION_H

#include <vector>
#include <climits>

/*!
  \brief Restituisce il valore intero, multiplo di 4, che segue immediatamente quello specificato

  e.g.
    correct_number_byte(5) = 8
    correct_number_byte(8) = 8

  @param val valore intero del quale si intende sapere il multiplo di 4 più vicino
  @param valore intero multiplo di 4 >= a quello specificato
*/
int correct_number_byte(int val);
/*!
  \brief Restituisce la rappresentazione binaria del carattere specificato in input

  @param c carattere del quale si intende sapere la rappresentazione binaria
  @return vettore di booleani che rappresenta il carattere in binario
*/
std::vector<bool> char_to_bitset(char c);

/*!
  \brief Trasforma la rappresentazione binaria passata in input
  in un carattere.

  @param bools rappresentazione binaria di un byte
  @return carattere risultante dall'interpretazione della sequenza di byte
*/
char bitset_to_char(std::vector<bool> bools);

#endif // UTILITYFUNCTION_H
