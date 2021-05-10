#ifndef HASH_H
#define HASH_H

#include <math.h>

class Bucket{
    public:
        unsigned int cpf;                       //chave de Hashing.
        unsigned int ponteiro;                  //Ponteiro para o arquivo.

        Bucket();
        ~Bucket();
};

class Pagina : public Bucket{
    private:
        size_t pTamanho;                        //size_t normalmente é recomendado para armazenar tamanhos de sizeof.
    public:
        unsigned int pBcket_MAX;                //quantidade máxima de Buckets por página.
        unsigned int pDepth;                    //profundidade da página.
        unsigned int pBuckets;                  //quantidade de buckets na página.
        Bucket *b;                              //ponteiro do Bucket.

        Pagina();
        Pagina(unsigned int tam);
        ~Pagina();
        
        /*
        * Aloca dinamicamente um espaço temporário para o ponteiro do Bucket
        * on comprimento a quantidade de Buckets alocados para determinada página.
        */
        void criarPagina(unsigned int tamanho); 

        /*
        * Deleta a página alocada.
        */
        void deletaPagina();

        /*
        * Retorna o tamanho da página.
        */
        size_t GetpTamanho(); 
};

class Hash{
    public:
        unsigned int Buckets_MAX;               //usado para definir a quantidae máxima de buckets por página.
        unsigned int profundidade;              //Profundidade do Hash.
        bool is_linked;                         //Flag para auxuilo na manipulação dos arquivos.
        Pagina pagina;                          //Página a ser escrita no arquivo de indices.

        /*
        * Função Hash, retorna um valor dado o CPF.
        */
        unsigned int hashFunction(unsigned int key);
        Hash();
        ~Hash();
};

#endif