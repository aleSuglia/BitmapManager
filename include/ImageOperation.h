#ifndef IMAGEOPERATION_H
#define IMAGEOPERATION_H

#include <vector>
#include <string>

/*!
  \brief Tipo enumerativo che rappresenta la tipologia di operazione
  che può essere svolta dal gestore di immagini.
*/
typedef enum{
  OPEN_IMAGE, ///< richiesta di apertura dell'immagine
  SAVE_IMAGE, ///< richiesta di salvataggio dell'immagine
  MODIFY_IMAGE ///< richiesta di modifica dell'immagine
}ImageOperation;

/*!
  \brief Tipo enumerativo che specifica la tipologia di effetti applicabili
  all'immagine correntemente visualizzata dal gestore.
*/
typedef enum {
    NOT_SUPPORTED, ///< tipo di effetto non supportato dal gestore
    NEGATIVE, ///< effetto negativo
    GRAYSCALE, ///< effetto scala di grigi
    SEPIA ///< effetto seppia
}ImageEffectOperation;

/*!
  \brief Restituisce il tipo enumerativo rappresentato dalla stringa passata in input
  rappresentante il nome del tipo di effetto da applicare.

  @param effect_name nome dell'effetto
  @return tipo enumrativo relativo al nome dell'effetto specificato
*/
ImageEffectOperation getEffectEnumValue(const std::string& effect_name);
/*!
  \brief Restituisce la lista degli effetti per l'immagine attualmente supportati
  dal gestore.

  @return lista degli effetti, in forma testuale, supportati dal gestore d'immagini
*/
std::vector<std::string> getSupportedFilters();



#endif // IMAGEOPERATION_H
