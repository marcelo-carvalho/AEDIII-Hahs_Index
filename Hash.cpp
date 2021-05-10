#include "Hash.h"

/*----------Implementação da classe Bucker----------*/

/*O construtor inicia o objeto com valores default
que são os mesmos valores que indicam um bucket vázio*/
Bucket::Bucket(){
    cpf = 0;
    ponteiro = 1;
}

Bucket::Bucket() {}

/*----------Implementação da classe Pagina----------*/

/*Construtor padrão*/
Pagina::Pagina(){
    pBcket_MAX = 4;
    pBuckets = 0;
    pDepth = 0;
}

/*Construtor overloaded definindo uma quantidade 
máxima de Buckets por página*/
Pagina::Pagina(unsigned int tamanho){
    pBcket_MAX = tamanho;
    pBuckets = 0;
    pDepth = 0;
}

Pagina::~Pagina() {}


void Pagina::criarPagina(unsigned int tamanho){
    this->b = new Bucket[tamanho];
}

void Pagina::deletaPagina(){
    delete[] this->b;
}

size_t Pagina::GetpTamanho(){
    this->pTamanho = (size_t) (sizeof(Bucket) * pBcket_MAX) + sizeof(pBuckets) + sizeof(pDepth);
    return this->pTamanho;
}

/*----------Implementação da classe Hash----------*/

Hash::Hash(){
    profundidade = 0;
    Buckets_MAX = 2;
}

Hash::~Hash(){}

unsigned int Hash::hashFunction(unsigned int key){
    return key % (unsigned int) pow(2, this->profundidade);
}