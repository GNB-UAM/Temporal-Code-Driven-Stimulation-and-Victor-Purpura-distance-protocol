////////////////////////////////////////////////////////////////////////////////
/*!
   \file VictorPurpura.h
   \date 7/2022
   \author Alberto Ayala

   \brief Archivo con las definiciones del modulo VictorPurpura

   \version 1.0
*/
////////////////////////////////////////////////////////////////////////////////
#include <default_gui_model.h>
#include <runningstat.h>
#define MAX_WORDS_BUFF 60000

#define NOT_INITIATED -2
#define ERR -1
#define OK 0

#define MAX_BITS_WORD 30
#define MAX_WORDS 1024
#define FIRST_CALL 0
#define NEW_TIME_WINDOW 1
#define MAX_WORD_TAM 1000

#define BIT_DETECTED_OUT 1
#define BIT_NOT_DETECTED_OUT 0
#define END_WINDOW_OUT 2
#define WORD_DETECTED_OUT 3
#define END_HISTOGRAM_TIME -2

#define FALSE 0
#define TRUE 1


//!< WbElement equivalente a entero
typedef int WbElement;

/*!
  * @struct      BitsBuffer
  * @brief   estructura usada para gestionar el buffer de bits
  */
typedef struct{
    char bits[MAX_BITS_WORD];//!< Buffer de bits
    int wordLength;//!< Tamanio de la palabra
    int numBits;//!< Numero de bits
    char* init;//!< Puntero al buffer
    char* insert;//!< Puntero de insercion al buffer
  } BitsBuffer;

 /*!
  * @struct      WordsBuffer
  * @brief   estructura usada para gestionar las palabras detectadas
  */ 
typedef struct {
    WbElement words[MAX_WORDS_BUFF]; //!< Buffer de palabras
    BitsBuffer bb; //!< estructura buffer de bits
    int maxWords; //!< maximo numero de palabras
    int numWords; //!< Primer voltaje analizada de la señal
    WbElement* init; //!< Puntero al buffer
    WbElement* insert; //!< Puntero de insercion al buffer
    WbElement* check; //!< Puntero a la palabra a comprobar   
  } WordsBuffer;

class VictorPurpura : public DefaultGUIModel
{

  Q_OBJECT

public:
  VictorPurpura(void);
  virtual ~VictorPurpura(void);

  void execute(void);
  void createGUI(DefaultGUIModel::variable_t*, int);
  void customizeGUI(void);

protected:
  virtual void update(DefaultGUIModel::update_flags_t);

private:
  //!< Periodo del sistema
  double period;
  //!< Tiempo en el que se ejecuto la tarea anterior
  long long lastTime;
  //!< Bit detectado
  char out;
  //!< Tiempo de bin
  long long bin;
  //!< Estructura usada para el analisis de las palabras
  WordsBuffer *wb = NULL;
  //!< Numero de ejecuciones de la tarea
  int numCalls;
  //!< Si se ha detectado la palabra o no
  short detectedWord;
  //!< Longitud de la palabra
  int length,maxWords;
  //!< Palabra a detectar
  char *word;
  //!< Palabra auxiliar 
  char newword[100];
  //!< Variable auxiliar
  int result;
  //!< Primer voltaje analizado de la señal
  double lastVolt1;
  //!< Ultimo voltaje analizado de la señal
  double lastVolt2;
  //!< Voltaje umbral
  double threshold;
  //!< longitud tren de spikes buscado
  int len1;
  //!< tren de spikes buscado
  double* s1d;
  //!< numero de codigos detectados
  int count;
  //!< numero de spikes
  int numsp;
  //!< parametro limite Victor-Purpura
  double limit;
  //!< parametro sensibilidad Victor-Purpura
  int q;
  //!< matriz calculo distancia Victor-Purpura
  int** matrix;
  //!< parametro depuracion
  int check;

  void initParameters();
  void charToInt();

private slots:
  int detect_spike (double voltage);
  short calculateDistance();
  double * intToDex(char * s,double tiempo_bin, int*len);
};
