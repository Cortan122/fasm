#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>

#include <pthread.h>
#include <semaphore.h>

// цветной вывод для 🌈юзабилити🌈
#define TIME_FORMAT "Время \x1b[32m%3ldms\x1b[0m: "
#define WRITER_FORMAT TIME_FORMAT "Писатель \x1b[96m%2ld\x1b[0m "
#define READER_FORMAT TIME_FORMAT "Читатель \x1b[34m%2ld\x1b[0m "
#define NTH_FORMAT " \x1b[33m%d\x1b[0mого "

const int DB_SZIE = 100000;
int* db = NULL;
pthread_mutex_t writerMutex;
sem_t readerSemaphore;
sem_t writerSemaphore;
long initialStartTime;

// тут неособо понятно как должна выглядить наша БД
// нужно придумать самый простой (KISS) способ сделать БД, в которой есть "отношения между данными" и противоречивые состояния
// очевидным решением будет 2 файла как симуляция двух таблицы, но это както слишком сложно
// всётаки кажется лучше будет выбрать дерево
// его можно очень просто хранить просто как масив индексов (в каждом элементе хранится индекс его родителя)
// и такой масив очень просто сериализировать
// в нём также могут быть "противоричивые состояния", например, когда у нас цикл или несколько корней

void genDb(){
  printf("генерируем базу данных\n");
  // db[0] unused; мы кидаем в него какойто recognizable value, чтобы было легче дебажить
  db[0] = 0xcafebeef;
  // тут вроде норм использовать rand() потомучто на linux-е (у меня) RAND_MAX=2147483647
  for(int i = 1; i < DB_SZIE; i++)db[i] = rand() % i;
}

void loadDb(){
  db = malloc(DB_SZIE * sizeof(int));

  FILE* file = fopen("db.bin", "r");
  if(!file){
    // perror("db.bin");
    genDb();
    return;
  }
  fread(db, sizeof(int), DB_SZIE, file);
  fclose(file);
}

void saveDb(){
  FILE* file = fopen("db.bin", "w");
  fwrite(db, sizeof(int), DB_SZIE, file);
  fclose(file);
  free(db);
}

// int findDepth(int index){
//   int res = 0;
//   for(int i = index; i != 0; i = db[i])res++;
//   return res;
// }

long getMilliseconds(){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

uint32_t getRandomSeed(){
  return getMilliseconds() ^ pthread_self();
}

long deltaTime(){
  return getMilliseconds()-initialStartTime;
}

bool isRelated(int id1, int id2){
  for(int i = id1; i != 0; i = db[i]){
    if(i == id2)return true;
  }
  for(int i = id2; i != 0; i = db[i]){
    if(i == id1)return true;
  }
  return false;
}

void writer(size_t id){
  uint32_t seed = getRandomSeed();
  int id1 = db[rand_r(&seed) % DB_SZIE];
  int id2 = db[rand_r(&seed) % DB_SZIE];

  printf(WRITER_FORMAT"приступил к замене"NTH_FORMAT"и"NTH_FORMAT"листа\n", deltaTime(), id, id1, id2);

  if(isRelated(id1, id2)){
    printf(WRITER_FORMAT"понял что"NTH_FORMAT"и"NTH_FORMAT"менять нельзя\n", deltaTime(), id, id1, id2);
    return;
  }

  for(int i = 0; i < DB_SZIE; i++){
    if(db[i] == id1){
      db[i] = id2;
    }else if(db[i] == id2){
      db[i] = id1;
    }
  }

  printf(WRITER_FORMAT"поменял"NTH_FORMAT"и"NTH_FORMAT"листы местами\n", deltaTime(), id, id1, id2);
}

void reader(size_t id){
  uint32_t seed = getRandomSeed();
  int id1 = db[rand_r(&seed) % DB_SZIE];

  printf(READER_FORMAT"приступил к подсчету количества детей"NTH_FORMAT"листа\n", deltaTime(), id, id1);
  int res = 0;
  for(int i = 0; i < DB_SZIE; i++){
    if(db[i] == id1)res++;
  }
  printf(READER_FORMAT"понял что у"NTH_FORMAT"листа %d детей\n", deltaTime(), id, id1, res);
}

void* writer_thread(void* a){
  // тут мы спим рандомное время чтобы сделать более интересным порядок вызова потоков
  uint32_t seed = getRandomSeed();
  usleep(rand_r(&seed) % 2000);

  pthread_mutex_lock(&writerMutex);
  int res;
  sem_getvalue(&readerSemaphore, &res);
  if(res != 0)sem_wait(&writerSemaphore);

  writer((size_t)a + 1);

  pthread_mutex_unlock(&writerMutex);
  return NULL;
}

void* reader_thread(void* a){
  uint32_t seed = getRandomSeed();
  usleep(rand_r(&seed) % 2000);

  pthread_mutex_lock(&writerMutex);
  pthread_mutex_unlock(&writerMutex);
  sem_post(&readerSemaphore);

  reader((size_t)a + 1);

  sem_wait(&readerSemaphore);
  int res;
  sem_getvalue(&readerSemaphore, &res);
  if(res == 0)sem_post(&writerSemaphore);
  return NULL;
}

void startThreads(size_t numWriters, size_t numReaders){
  size_t numThreads = numWriters+numReaders;
  pthread_t* threads = malloc(sizeof(pthread_t)*numThreads);

  size_t index = 0;
  for(size_t i = 0; i < numWriters; i++){
    pthread_create(threads+index++, NULL, writer_thread, (void*)i);
  }
  for(size_t i = 0; i < numReaders; i++){
    pthread_create(threads+index++, NULL, reader_thread, (void*)i);
  }

  for(size_t i = 0; i < numThreads; i++){
    pthread_join(threads[i], NULL);
  }
  free(threads);
}

int main(){
  initialStartTime = getMilliseconds();
  srand(time(NULL));
  loadDb();
  pthread_mutex_init(&writerMutex, NULL);
  sem_init(&readerSemaphore, 0, 0);
  sem_init(&writerSemaphore, 0, 0);

  startThreads(10, 10);

  saveDb();
  return 0;
}
