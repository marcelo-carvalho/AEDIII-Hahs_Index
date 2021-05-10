#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cstring>
#include "Hash.h"

/*Struct para encapsular os valores de data*/

typedef struct Data{
    unsigned short int dia;
    unsigned short int mes;
    unsigned short int ano;
};

class Registro{
    public:
        short record_size;      //tamanho de cada arquivo
        unsigned int cpf;
        std::string nome;
        Data aniversario;
        char sexo;
        std::string notas;

        Registro();
        ~Registro();

        /*Método para imprimier o registro*/
        void print(Registro p);
};

class Gerenciador{
    public:
        std::string arquivoPrincipal;
        std::fstream mainFile;
    protected: 
        long int offset;
};

class Indexador{
    public:
        std::string indexFileName;
        std::string directoryFileName;
        Hash hash;
        
        /*
        * Vetor que armazena o diretório na memória primaria. 
        * O diretório contém os endereços das páginas.
        */
        unsigned int* array;            

        /*
        * Método para buscar o bucket que contém o registro dado o endereço e o CPF.
        */
        unsigned int search(unsigned int address, unsigned int cpf);   

    protected:
        std::fstream indexFile;
        std::fstream directoryFile;

        /*
        * escreve uma página em memória secundária.
        */
        void writePage(Pagina p);

        /*
        * Lê uma página do qarquivo em memória secundária.
        */
        Pagina readPage();

        /*
        * Imprime uma página.
        */
        void printPage(Pagina p);

        /*
        * Lê o diretório, armazena o profundidada do diretório no membro "profundidade" de Hash e retorna
        * um ponteiro para um arrey que contem as referências das páginas do indice.
        */
        unsigned int* readDirectory();

        /*
        * Deleta o registro de uma página dado seu endereço e o CPF.
        */
        bool deleteCPF(unsigned int address, unsigned int cpf);

        /*
        * compara dois endeços de página.
        * True se forem iguais;
        * False caso contrário.
        * */
        bool compareAddresses(const int i, const int j);

        /*Calcula quantos bits no mínimo são necessarios para representar um número decimal.*/
        unsigned int nBits(unsigned int n);

        /*verifica se o resultado do rehash com determinado cpf ira permitir a mudança de pagina do bucket 
        referente ao cpf para a nova pagina a ser criada.*/
        bool is_linked(unsigned int root, unsigned int nbits, const unsigned int cpf, const unsigned int newBucketDepth);

        /*Atualiza o vetor do diretório em memória principal.*/
        void updateArray();
};

class GerenciadorArquivo : public Gerenciador, public Indexador, public Registro{
    
    private: 
        clock_t start_t, end_t;
        
        /*Define um offset baseado no tamanho de cada registro ao final da inserção.*/
        void setEOF(long int offset); 

        /*gerencia o arquivo diretorio.*/
        Bucket LinkIndexToMain(unsigned int pos, Bucket b);

        /*gerencia o arquivo diretorio*/
        void LinkDirectoryToIndex(unsigned int cpf, unsigned int address);

        /*rearranja os ponteiros do diretório sempre que preciso.*/
        void RearrangePointers(unsigned int root, unsigned int nbits, unsigned int address);

        /*Printa o diretório.*/
        void printDirectoryFile();

        /*Imprime o arquivo de indices.*/
        void printIndexFile();
    
    public:
        GerenciadorArquivo();
	    ~GerenciadorArquivo();

        /*Função para adicionar um registro.*/
        bool addRecord(Registro r);

        /*Função para escrever o registro no arquivo mestre.*/
        unsigned int writeRecord(Registro r);
        
        /*Lê um registro do arquivo mestre dado o seu endereço(offset a partir do inicio).*/
        Registro readRecord(unsigned int offset);

        /*Edita determinado registro dado o cpf e as notas.*/
        bool edit(unsigned int cpf, std::string notes);

        /*printa o arquivo mestre.*/
        void printMainFile();

        /*Função que da acesso ao delete.Remove um registro.*/
        bool remove(unsigned int cpf);

        /*Imprime todos os arquivos.*/
        void printFiles();

        /*Cria um sistema de três arquivos e reseta o conteudo anterior destes.*/
        void createFiles();
};


#endif