////////////////////////////////////////////////////////////////////////////////
/*!
 * @file VictorPurpura.cpp
 * @date 7/2022
 * @author Alberto Ayala
 * 
 * @brief Archivo protocolo VictorPurpura
 * 
 * Este modulo detecta códigos neuronales y compara 
 * con uno buscado mediante el calculo de la distancia de Victor-Purpura
*/
////////////////////////////////////////////////////////////////////////////////

#include "Victor-Purpura-TCDS-closed-loop-stimulation-protocol.h"
#include <iostream>
#include <main_window.h>
#include <math.h>
#include <string.h>
#include <stdio.h>


int wbInit(WordsBuffer* wb, int length, int maxWords);
void bbAdvancePtr (BitsBuffer* bb, char** ptr);
int wbBitInsert(WordsBuffer* wb, char bit);
int Bits2Int(char* bb, int length);
int wbBits2Int(WordsBuffer *wb);
int wbWordInsert (WordsBuffer* wb, int word);
int wbStoreWord (WordsBuffer *wb);
int wbCheckWordMatch(WordsBuffer *wb, char *word);
int calculateDistance();
void bbReversePtr (BitsBuffer* bb, char** ptr);

extern "C" Plugin::Object*
createRTXIPlugin(void)
{
  return new VictorPurpura();
}

static DefaultGUIModel::variable_t vars[] = {
  { "IV", "Weakly electric fish voltage (V)", DefaultGUIModel::INPUT, },
  { "Now", "Task startup time", DefaultGUIModel::OUTPUT, },
  { "output", "Stimulus enable flag", DefaultGUIModel::OUTPUT, },
  { "bit detected", "Bit detected within current time window", DefaultGUIModel::OUTPUT, },
 
  { "Bin Time (ns)", "Time window",
  DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },

  { "Threshold (V)", "Minimum voltage to detect a spike",
  DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },

  { "Word length", "Number of bits of the word",
  DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },

  { "q", "Relative sensitivity of the metric to the precise timing of the spikes",
  DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },

  { "Limit", "Victor Purpura distance threshold",
  DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },

  { "Word", "Word to detect",
  DefaultGUIModel::PARAMETER, },
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

VictorPurpura::VictorPurpura(void)
  : DefaultGUIModel("Victor Purpura and TCDS closed loop stimulation protocol", ::vars, ::num_vars)
{
  setWhatsThis("Implementation of a real-time closed-loop stimulation protocol to study activity codes with flexibility using the Real-Time eXperiment Interface (RTXI). This new and easier-to-use implementation is based on the TCDS protocol and the Victor-Purpura distance.");
  DefaultGUIModel::createGUI(vars,
                             num_vars); // this is required to create the GUI
  customizeGUI();
  initParameters();
  update(INIT); // this is optional, you may place initialization code directly
                // into the constructor
  refresh();    // this is required to update the GUI with parameter and state
                // values
  QTimer::singleShot(0, this, SLOT(resizeMe()));
}

VictorPurpura::~VictorPurpura(void)
{
}

void
VictorPurpura::execute(void)
{

  long long now;
  now = RT::OS::getTime();//nanoseconds

  int salida;
  salida = 0;
  char bit_d;
  bit_d = -1;
  int r;
  r =-2;

  double voltage = input(0);
  output(0) = now;

  if(numCalls==NEW_TIME_WINDOW){
    lastTime = now;
    numCalls ++;
  }

  if(detect_spike(voltage)&& out!=BIT_DETECTED_OUT){
    numsp++;

    
    out = BIT_DETECTED_OUT;
    bit_d = out;
    
    wbBitInsert(wb,out); //store 1 bit array
    result = wbStoreWord(wb);
    r = result;

    if(ERR != result){
      check = *(wb->check);

      detectedWord = calculateDistance();
    }
    
  }else{
  }

  if(bin<(now-lastTime)){
    if(out == BIT_NOT_DETECTED_OUT){
      bit_d = out;
      wbBitInsert(wb,out); //store 0 bit array
      result =wbStoreWord(wb);
      r = result;
      if(ERR != result){
        check = *(wb->check);

        detectedWord = calculateDistance();
      }
    }

    out = BIT_NOT_DETECTED_OUT;
    lastTime = now;
    numCalls ++;


    if(detectedWord){

      salida = 1;
      detectedWord = FALSE;
    }
    
  }else{
  }
  output(1) = salida;
  output(2) = bit_d;

  return;
}

void
VictorPurpura::initParameters(void)
{
  out = BIT_NOT_DETECTED_OUT;
  detectedWord = FALSE;
  result = -2;
  q = 25;
  count = 0;
  numsp = 0;

  check = -8;

  if (wb){
    free(wb);
  }
  length = 30;
  wb = (WordsBuffer*)malloc(sizeof(WordsBuffer));
  maxWords = 1;
  wbInit(wb,length,maxWords);
  numCalls = NEW_TIME_WINDOW;
  lastVolt1 = 0;
  lastVolt2 = 0;
  //latency = 0;
  //timestep = 0;
  //lastRead = 0;
  //firstRead = true;
}

void
VictorPurpura::update(DefaultGUIModel::update_flags_t flag)
{
  switch (flag) {
    case INIT:
      limit = 2.5;
      bin = 20000000;
      threshold = 3;

      word = (char*)calloc(length+1,sizeof(char));
      strcpy(word, "111010100010100010100010101000");
      
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      setParameter("Word length", length);
      setParameter("Threshold (V)", threshold);
      setParameter("Bin Time (ns)", bin);
      setParameter("Word", word);
      setParameter("q", q);
      setParameter("Limit", limit);
      charToInt();
      word = newword;
      char* s1;
      s1 = word;
      s1d = intToDex(s1,bin/1000000000,&len1);
      break;

    case MODIFY:
      initParameters();
      length = getParameter("Word length").toInt();
      q = getParameter("q").toInt();
      threshold = getParameter("Threshold (V)").toDouble();
      bin = getParameter("Bin Time (ns)").toLong();
      word = (char*)getParameter("Word").toStdString().c_str();
      limit = getParameter("Limit").toDouble();
      charToInt();
      word = newword;
      
      break;

    case UNPAUSE:
      break;

    case PAUSE:
      break;

    case PERIOD:
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      break;

    default:
      break;
  }
}

void
VictorPurpura::customizeGUI(void)
{
  QGridLayout* customlayout = DefaultGUIModel::getLayout();
  setLayout(customlayout);
}

/**
 * Inicializa los valores y punteros de la estructura WordsBuffer
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @param[in,out]   s      tren de spikes a transformar
 * @param[out]       len   longitud del tren de spikes 
 * @param[in]       tiempo_bin tiempo de bin codificacion binaria 
 * @return     tren de spikes transformado

*/
double * VictorPurpura::intToDex(char * s,double tiempo_bin, int*len){

    int i,cont;
    cont = 0;
    int l1;
    double *result;
    result = (double*)calloc(length,sizeof(double));

    for(i = 0;i<length;i++){
   
      if(s[i]==1){
        result[cont] = (i)*tiempo_bin;
        
        cont++;
      }
      
    }
    *len = cont;
    return result;

}

/**
 * Calcula la distancia de Victor-Purpura
 * 
 * @date 7/2022
 * 
*/
short VictorPurpura::calculateDistance(){

  double **matrix;  
  double* s2d;
  int i,j;
  char* s1;
  char* s2;
  int len2;
  double min1,min2,min3,min;
  short detectWord;
  char* auxPtr;


  s1 = word;
  s2 = (char*)calloc(length,sizeof(char));
  auxPtr=wb->bb.init;
  bbReversePtr(&(wb->bb), &auxPtr);
    for (i=0; i<wb->bb.wordLength; ++i){
        if ((int)*auxPtr){
          s2[i] = 1;
        }else{
          s2[i] = 0;
        }

        bbAdvancePtr(&(wb->bb), &auxPtr);
    }

  s2d = intToDex(s2,bin/1000000000,&len2);
  if (len2 == 0){
    return 0;
  }
  matrix = (double**)calloc(len1+1, sizeof(double*));

    for(int i = 0;i<len1+1;i++) {
        matrix[i] = (double*)calloc(len2+1, sizeof(double));
    }

    for (j = 0;j<len2+1;j++){
      matrix[0][j] = j;
    }

    for (i = 0;i<len1+1;i++){
      matrix[i][0] = i;
    }

  for(i=1;i<len1+1;i++){
    for (j = 1;j<len2+1;j++){
      min1 = matrix[i-1][j] + 1;
      min2 = matrix[i][j-1] + 1;
      min3 = matrix[i-1][j-1] + q*abs(s1d[i-1]-s2d[j-1]);
      if(min1<=min2){
        min = min1;
      }else{
        min = min2;
      }
      if(min3<min){
        min = min3;
      }
      matrix[i][j] = min;
    }
  }

  if(matrix[len1][len2]<=limit){
      detectWord = 1;
      count++;
  }else{
    detectWord = 0;
  }


  for(int i = 0; i < len1+1; i++){
    free(matrix[i]);
  }  
  free(matrix);

  return detectWord;

}



/**
 * Detecta si hay un pulso por encima de un umbral determinado
 * 
 * @date 7/2020
 * 
 * @return 1 si detecta pulso, 0 en otro caso
*/
int VictorPurpura::detect_spike (double voltage)
{
    
    int spike=0;
    
    if ((lastVolt2<=lastVolt1) &&
        (lastVolt1>=voltage) &&
        (lastVolt1>threshold))
    {
        spike=1;
    }

    lastVolt2 = lastVolt1;
    lastVolt1 = voltage;

    return spike;
}



/**
 * Inicializa los valores y punteros de la estructura WordsBuffer
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @param[in,out]   wb       puntero a la estructura a rellenar
 * @param[in]       length   longitud de palabras en estructura expresada en bits    
 * @param[in]       maxWords máximo de palabras a almacenar en la estructura  
*/
int wbInit(WordsBuffer* wb, int length, int maxWords){
    wb->insert=wb->words;
    wb->init=wb->words;
    wb->check = wb->words;
    wb->bb.wordLength=length;
    wb->numWords=0;
    wb->maxWords=maxWords; 
    wb->bb.init = wb->bb.bits;
    wb->bb.insert = wb->bb.bits;
    wb->bb.numBits = 0;
	
    return OK;
}

/**
 * Avanza el puntero en la estructura de bits, convirtiéndola en un buffer circular
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @param[in]   wb      puntero a la estructura a rellenar
 * @param[in,out]       ptr     puntero a avanzar
*/
void bbAdvancePtr (BitsBuffer* bb, char** ptr){
    if (*ptr==&(bb->bits[bb->wordLength-1])) *ptr = bb->bits;
    else (*ptr)++;
}
void bbReversePtr (BitsBuffer* bb, char** ptr){
    if (*ptr==bb->bits) *ptr = &bb->bits[bb->wordLength-1];
    else (*ptr)--;
}

/**
 * Inicializa los valores y punteros de la estructura WordsBuffer
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @param[in,out]   wb      puntero a la estructura a rellenar
 * @param[in]       bit     bit a introducir
*/
int wbBitInsert(WordsBuffer* wb, char bit){  //Inserts bit on BitBuffer
    *(wb->bb.insert)=bit;
    bbAdvancePtr(&(wb->bb), &(wb->bb.insert));
    wb->bb.numBits++;
    return OK;
}

int Bits2Int(char* bb, int length){ 
	char *auxPtr;
	int i, exp;
	int wordResult = 0;
	
	exp=length - 1;
	
	auxPtr=bb;
    for (i=0; i<length; ++i){
        if ((int)*auxPtr){
            switch (exp){
                case 0: wordResult += 1; break;
                case 1: wordResult += 2; break;
                default: wordResult += pow(2,exp); break;
            }
        }
        exp--;
        auxPtr++;
    }
    
    return wordResult;
}

int wbBits2Int(WordsBuffer *wb){ 
	char *auxPtr;
	int i, exp;
	int wordResult = 0;
	
	exp=wb->bb.wordLength - 1;
	
	auxPtr=wb->bb.init;
    for (i=0; i<wb->bb.wordLength; ++i){
        if ((int)*auxPtr){
            switch (exp){
                case 0: wordResult += 1; break;
                case 1: wordResult += 2; break;
                default: wordResult += pow(2,exp); break;
            }
        }
        exp--;
        bbAdvancePtr(&(wb->bb), &auxPtr);
    }
    
    return wordResult;
}



/**
 * Inserta una palabra con el valor de word en la estructura WordsBuffer
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @param[in,out]   wb      puntero a la estructura a rellenar
 * @param[in]       word    valor de la palabra
*/
int wbWordInsert (WordsBuffer* wb, int word){ //Inserts word on WordsBuffer
                                                 //Called by StoreWord

    *wb->insert = word; //copy word value
	wb->check=wb->insert;
    wb->numWords++; 

    if (wb->insert == wb->words+wb->maxWords-1){
        wb->insert = wb->words;
        if (wb->init == wb->words+wb->maxWords-1) wb->init = wb->words;
        else wb->init++;
    } else wb->insert++;

    return OK;
}

/**
 * Inserta la palabra almacenada en el buffer de bits de la estructura en el buffer de palabras.
 * Para ello primero realiza una transformacion de bits a entero.
 * Si no se han introducido suficientes bits para establecer una palabra, devuelve ERR.
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @see wbWordInsert
 * 
 * @param[in,out]   wb      puntero a la estructura donde se insertará la palabra
 * 
 * @return retorna el valor entero de la palabra o ERR
*/
int wbStoreWord (WordsBuffer *wb){ 
    int wordResult;
    if (wb->bb.numBits<wb->bb.wordLength) return ERR;
    wordResult = wbBits2Int(wb);

    //Advance bit init
    bbAdvancePtr(&(wb->bb),&(wb->bb.init));

    //Insert word
    wbWordInsert(wb, wordResult);

    return wordResult;
}


/**
 * Comprueba si la palabra binaria word coincide con la que se encuentra en la estructura de bits
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @param[in]   wb      puntero a la estructura donde se encuentran los bits de una palabra a comparar
 * @param[in]   word    array de bits con la otra palabra a comparar
*/
int wbCheckWordMatch(WordsBuffer *wb, char *word){
	return (*(wb->check)==Bits2Int(word, wb->bb.wordLength));
}

/**
 * Transforma la palabra a analizar de ASCII a entero
 * 
 * @date 7/2020
 * 
*/
void
VictorPurpura::charToInt(){
  char h;
  for(int i=0;i<length;i++){
    h = (int)word[i] - 48;
    newword[i] = h;
  }

}
