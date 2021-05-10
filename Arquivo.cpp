#include "Arquivo.h"


/*------------------------------------------------Implementação da classe Registro------------------------------------------------*/

Registro::Registro(){
    cpf = 0;
    aniversario.dia = aniversario.mes = aniversario.ano = 0;
    sexo = '*';
}

Registro::~Registro() {}

void Registro::print(Registro p){
    std::cout << "CPF:" << p.cpf << std::endl;
    std::cout << "Nome: " << p.nome << std::endl;
    std::cout << "Aniversário: " << p.aniversario.dia << "/" << p.aniversario.mes << "/" << p.aniversario.ano << std::endl;
    std::cout << "Sexo: " << p.sexo << std::endl;
    std::cout << "Notas: " << std::endl;
    std::cout << p.notas << std::endl;
}

/*-------------------------------------------Implementação da classe GerenciadorArquivo-------------------------------------------*/

GerenciadorArquivo::GerenciadorArquivo(){
    arquivoPrincipal = "main.txt";
    indexFileName = "index.txt";
    directoryFileName = "directory.txt";
    record_size = 2500;
    hash.Buckets_MAX = 2;
    offset = 0;
    array = readDirectory();
}

GerenciadorArquivo::~GerenciadorArquivo(){
    if(array != NULL)
        free(array);
}

void GerenciadorArquivo::createFiles(){

    /*
    * Verifica se não existem arquivos aberto, caso encontre o mesmo é fechado.
    */
    if(directoryFile.is_open())
        directoryFile.close();
    if(mainFile.is_open())
        mainFile.close();
    if(indexFile.is_open())
        indexFile.close();

    /*VERIFICAR O QUE A FUNÇÃO OPEN ESTÁ FAZENDO E EXPLICAR NO COMENTÁRIO*/
    directoryFile.open(directoryFileName.c_str(), std::ios::trunc | std::ios::out | std::ios::in);
	indexFile.open(indexFileName.c_str(), std::ios::trunc | std::ios::out | std::ios::in);
	mainFile.open(arquivoPrincipal.c_str(), std::ios::trunc | std::ios::out | std::ios::in);

    /*
    * Fecha todos os arquivos.
    */
    mainFile.close();
	indexFile.close();
	directoryFile.close();

    hash.profundidade = 0;      //atribui o valor inicial para a profundidade global. 

    free(array);                //libera o espaço em memóaria principal do vetor do diretório. 
    array = NULL;               //Atribui uma referência nula para array.
}

void GerenciadorArquivo::setEOF(long int offset){
    while(offset != 0){
        mainFile << "\0";
        offset--;
    }
}

unsigned int GerenciadorArquivo::writeRecord(Registro r){
    size_t size;

    if(!mainFile.is_open()){
        /*
        * abre o arquivo para operações em binário de escrita.
        */
        mainFile.open(arquivoPrincipal.c_str(), std::ios::out | std::ios::binary | std::ios::in);
        mainFile.seekp(0, std::ios::end); //PESQUISAR O QUE O IOS FAZ**********************
    }

    if (mainFile.is_open() && mainFile.good()){
        
        unsigned int address = mainFile.tellp(); //salva o endereço do registro a ser inserido

        /*
        * Nome e Notas são strings que podem ter tamanhos variados, uma verificação 
        * precisa ser feita para saber se podem ser insiradas dentro do espaço
        * disponível, logo a seguinte compração é feita:
        * r.record_size = tamanho do registro 
        * caso as duas strings caibam no registro a condição abaixo será satisfeita. 
        */
        if (r.nome.size() + r.notas.size() <= (r.record_size - (sizeof(r.aniversario) + sizeof(r.cpf) + sizeof(r.sexo) + 2 * (sizeof(size))))){
            mainFile.write((char*) &r.cpf, sizeof(r.cpf));
            size = r.nome.size();                                           //Obtem tamanho da string nome.
            mainFile.write((char*) &size, sizeof(size_t));                  //Salva o tamanho da string nome.
            mainFile.write(r.nome.c_str(), r.nome.size());                  //Salva o conteúdo de nome.
            mainFile.write((char*) &r.sexo, sizeof(sexo));                  //Salva o caracter que define o sexo.
            mainFile.write((char*) &r.aniversario, sizeof(aniversario));    //Salva a data de nascimento. 
            size = r.notas.size();                                          //Obtém o tamanho de notas.
            mainFile.write((char*) &size, sizeof(size_t));                  //Salva o tamanho da string notas.
            mainFile.write(r.notas.c_str(), r.notas.size());                //Salva o conteúdo de notas.

            /*
            * Offset armazena a quantidade de bytes restante para completar o tamanho máximo do registro e 
            * assim deslocar corretamento o EOF.
            */
            offset = record_size - (sizeof(r.aniversario) + sizeof(r.cpf) + sizeof(r.sexo) + r.nome.size() + r.notas.size());

            /*
            * Desloca o EOF para posição tal que o registro apresenta a quantidade fixa de bytes.
            */
            setEOF(offset); 
        }

        /*
        * Caso não exista espaço o bastante para inserir as duas strings comples, estas serão inseridas até ser completo o espaço máximo.
        * Está inserção será feita caracter por caracter, podendo causar truncamento das informações do campo notas.
        */
       else{
           mainFile.write((char*)&r.cpf, sizeof(r.cpf));                    //Salva o conteúdo de CPF.
           size = r.nome.size();                                            //Obtém o tamanho da string nome.
           mainFile.write((char*)&size, sizeof(size_t));                    //Salva o tamanho da string nome.

           /*Calcula o espaço disponível */
           int availableSpace = (r.record_size - (sizeof(r.aniversario) + sizeof(r.cpf) + sizeof(r.sexo) + sizeof(size)));

           for(int i = 0; i < r.nome.length() && i < availableSpace; i++){
               /*
               * Insere a string nome caracter por caracter.
               */
               mainFile.write((char*)&r.nome.at(i), sizeof(char));
               availableSpace = availableSpace -1;                          //Decrementa 1 do espaço disponível a cada iteração.
           }

           mainFile.write((char*) &r.sexo, sizeof(r.sexo));                 //Salva o conteúdo de sexo.
           mainFile.write((char*) &r.aniversario, sizeof(aniversario));     //Salva o conteúdo de aniversario.

           availableSpace - availableSpace - sizeof(size);
           size = availableSpace;                                           //Tamanho restante para escrita de notas.

           for(int i =0; i < availableSpace; i++){
               /*
               * Insere a string notas caracter por caracter.
               */
               mainFile.write((char*)&r.notas.at(i), sizeof(char));        
           }

           std::cout << "AVISO: Tamanho máximo do registro atingido, informações poderão ser perdidas." << std::endl;
       }

       mainFile.close();
       return address;                                                      //Retorna o endereço onde o registro foi escrito.
    }
    else{
        std::cout << "ERROR: Arquivo não pode ser aberto." << std::endl;
        mainFile.close();
        return 1;                                                           //Retorna 1 em caso de falha.
    }
}

bool GerenciadorArquivo::addRecord(Registro r){
    start_t = clock();

    unsigned int address = writeRecord(r);

    if(address != 1){
        /*
        * Chama a função responsável por manipular o diretório.
        */
        LinkDirectoryToIndex(r.cpf, address);
    }
    else{
        return false;
    }

    end_t = clock();

    /*
    * Obs.: Rotina para simulação, apenas para teste de tempo.
    */

    /*std::cout.precision(5);
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout << (float) (end_t - start_t) / CLOCKS_PER_SEC << "s para escrever no arquivo" << std::endl;*/
    
}

Registro GerenciadorArquivo::readRecord(unsigned int offset){
    Registro tmp;

    if(!mainFile.is_open()){
        mainFile.open(arquivoPrincipal.c_str(), std::ios::in | std::ios::binary);
        mainFile.seekg(offset, std::ios::beg);                      //Desloca o ponteiro para a posição onde começa o registro. 
    }

    if(mainFile.is_open()){
        size_t size;                                                //Variável para armazenar o tamanho das strings nome e notas. 

        mainFile.read((char*) tmp.cpf, sizeof(tmp.cpf));            //Lê o CPF.
        mainFile.read((char*) &size, sizeof(size));                 //Lê o tamanho da string nome.
        tmp.nome.resize(size);                                      //Redimensiona o tamanho da string nome.
        mainFile.read((char*) tmp.nome.c_str(), size);              //Lê a string nome do arquivo.
        mainFile.read((char*) &tmp.sexo, sizeof(tmp.sexo));         //Lê o caracter de sexo. 
        mainFile.read((char*) &tmp.aniversario, sizeof(Data));      //Lê a data de nascimento.
        mainFile.read((char*) &size, sizeof(size)) ;                //Lê o tamanho da string notas.
        mainFile.read((char*) tmp.notas.c_str(), size);             //Lê a string notas do arquivo.
    }

    return tmp;                                                     //Retorna o registro lido.
}


bool GerenciadorArquivo::edit(unsigned int cpf, std::string notes){
    mainFile.open(arquivoPrincipal.c_str(), std::ios::out | std::ios::in | std::ios::binary);       //Abre o arquivo para edição.

    start_t = clock();                                              //Inicia o marcador de tempo para edição.

    /*
    * Verifica se o array do diretório é nulo. Se este for nulo, o arquivo é finalizado e 
    * falso é retornado pela função indicando que a edição no poderá ocorrer.
    */
   if(array == NULL){
       mainFile.close();
       return false;
   }

    unsigned int indexAddress = array[hash.hashFunction(cpf)];      //Busca o endereço da página onde o regsitro (cpf) pode ser encontrado.
    unsigned int mainAddress = search(indexAddress, cpf);           //Buscando o endereço para buscar a página no arquivo mestre. 

    /*
    * Se af unção search retornar 1, significa que não possível encontrar a chave no arquivo de indices.
    * Encerra-se a rotina e encerra o arquivo e retorna falso indicando que a edição não pode ser 
    * realizada.
    */
   if(mainAddress == 1){
       mainFile.close();
       return false;
   }

   Registro r;

   mainFile.seekg(mainAddress, std::ios::beg);                      //Posiciona o ponteiro do arquivo para posição do registro a ser editado.
   r = readRecord(mainAddress);                                     //Lê o registro.

   end_t = clock();                                                 //marca o fim do tempo para caluclar o tempo de edição.

   std::cout.precision(5);                                          //configura precisão de 5 casas decimais no tempo de pesquisa.
   std::cout.setf(std::ios::fixed, std::ios::floatfield);           //configuração de para saída de variáveis do tipo float.


   std::cout << (float) (end_t - start_t) / CLOCKS_PER_SEC << "s para editar o registro." << std::endl;
   std::cout.unsetf(std::ios::floatfield);                          //Restaura configuração de tipos float.

   r.notas = notas;                                                 //Atribui a string notas do registro a edição a ser feita.
   mainFile.seekg(mainAddress, std::ios::beg);                      //Reposiciona o ponteiro para a posição inicial do registro a ser editado.

   writeRecord(r);                                                  //Reescreve o registro lido.

   return true;
}

bool GerenciadorArquivo::remove(unsigned int cpf){
    if(array == NULL){
        return false;
    }

    unsigned int indexAdrres = array[hash.hashFunction(cpf)];       //Busca o endereço da página onde o regsitro (cpf) pode ser remocido.

    return deleteCPF(indexAdrres, cpf);                             //Chama a função responsável por fazer a remoção lógica do registro.
}

void GerenciadorArquivo::printMainFile(){
    Registro tmp;
    int offset = 0;

    mainFile.open(arquivoPrincipal.c_str(), std::ios::in | std::ios::binary);       //Abre o arquivo para leitura.

    if(mainFile.is_open()){
        if(mainFile.peek() != EOF){                                 //Verifica se o arquivo está vázio.
            while(mainFile.peek() != EOF){                          //Repete o loop até o fim do arquivo.
                mainFile.seekg(offset, std::ios::beg);              //Posiciona o ponteiro para fazer a leitura do registro.
                if(mainFile.peek() != EOF){                         //Verifica se não é o fim do arquivo.
                    tmp = readRecord(offset);

                    /*
                    * Verifica se é um registro válido e presente no índice.
                    */
                    if(search(array[hash.hashFunction(tmp.cpf)], tmp.cpf) != 1){
                        print(tmp);                                 //Imprime o registro.
                        std::cout << std::endl;
                    }
                }
                offset = offset + record_size;                      //Desloca o offset para o registro seguinte.
            }                           
        }
        else{
            std::cout << "AVISO: Arquivo vázio." << std::endl;
        }
    }
    else{
        std::cout << "AVISO: Não foi possível imprimir o arquivo." << std::endl;
    }

    mainFile.close();
}

void GerenciadorArquivo::printDirectoryFile(){
    if(array == NULL){
        std::cout << "AVISO: Arquivo vázio." << std::endl;
    }
    else{
        for(int i = 0; i < pow(2,hash.profundidade); i++){
            std::cout << "[" << i << "] = ";
            std::cout.setf(std::ios::hex, std::ios::basefield);     //Configura a saída dos valores para hexadecimal.
            std::cout.setf(std::ios::showbase);
            std::cout << array[i] << std::endl;                     //Imprime o vetor do diretório.
            std::cout.unsetf(std::ios::hex | std::ios::showbase);
        }
    }
}

void GerenciadorArquivo::printIndexFile(){
    Pagina p;
    int i = 1;

    if(indexFile.is_open()){
        indexFile.close();
    }

    indexFile.open(indexFileName.c_str(), std::ios::in | std::ios::binary);

    if(indexFile.is_open()){
        if(indexFile.peek() != EOF){
            while(indexFile.peek() != EOF){
                std::cout << "Pagina " << i << std::endl;
                p = readPage();                                         //Lê uma página.
                printPage(p);                                           //Imprime a página.
                p.deletaPagina();
                i++;
            }
        }
        else{
            std::cout << "AVISO: Arquivo está vázio." << std::endl;
        }
    }
    else{
        std::cout << "ERRO: Arquivo não pode ser impresso." << std::endl;
    }
    indexFile.close();
}

void GerenciadorArquivo::printFiles(){
    /*
    * Função para imprimer todos os arquivo de uma única vez.
    */

   std::cout<< "Arquivo de Diretório: " << std::endl;
   printDirectoryFile();
   std::cout << std::endl;

   std::cout << "Arquivo Índice: " << std::endl;
   printIndexFile();
   std::cout << std::endl;

   std::cout << "Arquivo Principal: " << std::endl;
   printMainFile();
}

void GerenciadorArquivo::LinkDirectoryToIndex(unsigned int cpf, unsigned int address){
    if(directoryFile.is_open()){
        directoryFile.close();
    }

    directoryFile.open(directoryFileName.c_str(), std::ios::in | std::ios::binary);

    Bucket bucket;
    Bucket b;

    b.cpf = cpf;
    b.ponteiro = address;

    if(directoryFile.peek() == EOF){                                                            //Verifica se o arquivo está vázio.
        directoryFile.close();
        directoryFile.open(directoryFileName.c_str(), std::ios::out | std::ios::binary | std::ios::app);
        if(directoryFile.is_open()){
            directoryFile.write((char*)&hash.profundidade, sizeof(hash.profundidade));          //Escreve a profundidade inicial do diretório, no caso 0.
            
            /*
            * A função LinkIndexToMain retorna um bucket que é usado como sinalizado para alguma operação interna.
            * Quando bem sucedido em realizar o link entre o mains e o diretório, a função retorna o endereço da
            * página onde o bucket foi inserido, através do membro "ponteiro" da classe Bucket.
            */
            bucket = LinkIndexToMain(0,b);                                          //Chama a função que manipula o arquivo de índices para inserir o nome bucket na página.

            directoryFile.write((char*) &bucket.ponteiro, sizeof(unsigned int));    //Salva o endereço da primeira página.
            directoryFile.close();
            updateArray();                                                          //Atualiza o vetor do diretório em memória primária.

        }

    }
    else{
        directoryFile.close();

        /*
        * O comando a seguir chama o método responsável por armazenar o novo bucker no índice e fazer o link
        * entre o arquivo mestre e o índice. 
        * A função tem como argumento o endereço da página e o bucket a ser inserido.
        * Internamente o método manipula a flag is_linked da classe Hash. Caso essa flig seja falsa, o
        * bucket não pôde ser inserido.
        */
       bucket = LinkIndexToMain(array[hash.hashFunction(cpf)], b);

       
       if(!hash.is_linked){
            /*
            * Caso a função retorne um bucket cujo membro "ponteiro" é igual a 1, significa que é necessário fazer a 
            * expansão do diretório.
            */
           if(bucket.ponteiro == 1){
               unsigned int* indicesArray;                          //Ponteiro temporário para geração do novo bloco de memáoria contendo o diretório.
               int i = 0;

               free(array);                                         //Reseta o array do diretório antes de fazer a alicação do novo diretório.
               array = NULL;

               directoryFile.open(directoryFileName.c_str(), std::ios::in | std::ios::binary);
               hash.profundidade++;                                 //Aumenta a profundidade global da Hash. 

               /*
               * Aloca o novo bloco de memória do diretório.
               */
               indicesArray = (unsigned int*)malloc(pow(2,hash.profundidade) * sizeof(unsigned int));        

               if(indicesArray == NULL){
                   std::cout << "ERRO: Impossivel alocar memória para o vetor do diretório." << std::endl;
                   exit(0);
               }

                /*
                * Define a posição incial do ponteiro para o conteúdo antigo do diretório, exceto a profundidade.
                */
               directoryFile.seekg(4,std::ios::beg);

               while(directoryFile.peek() != EOF){
                   directoryFile.read((char*) &indicesArray[i], sizeof(unsigned int));
                   i++;
               }

               directoryFile.close();

               while(i < (int)pow(2,hash.profundidade)){
                   /*
                   * Aponta pra as novas posições geradas na expansão do diretório. 
                   * O apontamento funciona da seguinte forma:
                   * 
                   * Posição_nova = (primeira posicação nova) - ((2^profundade) / 2)
                   */

                  indicesArray[i] = indicesArray[i - (int)(pow(2,hash.profundidade) / 1)];
                  i++;
               }

               directoryFile.open(directoryFileName.c_str(), std::ios::out | std::ios::binary);
               directoryFile.write((char*) &hash.profundidade, sizeof(hash.profundidade));                      //Escreve a nova profundidade.
               directoryFile.write((char*)indicesArray, sizeof(unsigned int) * pow(2,hash.profundidade));       //Escreve o novo diretório.

               directoryFile.close();
               free(indicesArray);
               updateArray();                            //Atualiza o diretório em memória primária.

               LinkDirectoryToIndex(cpf,address);       //Tenta inserir o bucket novamente (chamada recursiva).
           }
           else{
               /*
               * Caso a função retorne diferente de 1, singicia que ela retornou um endereço de uma nova página
               * porém ainda não foi possível inserir o bucket, pois todos os elementos estão na mesma página 
               * após o rehash. Sendo assim é necessário fazer a reordenação dos ponteiros por conta da nova página.
               */

                RearrangePointers(bucket.cpf, nBits(bucket.cpf) + 1, bucket.ponteiro);    //Faz o rearranjo dos ponteiros do diretório.

              /*
               * OBS.: O método linkIndexToMain atribui ao CPF uma posição do vetor do diretório no qual o endereço
               * da nova página deva ser atribuído.
              */

                array[bucket.cpf] = bucket.ponteiro;    //Atribui a posição do vetor do diretório o endereço da nova página.

                directoryFile.open(directoryFileName.c_str(), std::ios::out | std::ios::binary);

                directoryFile.write((char*) &hash.profundidade, sizeof(hash.profundidade));         //Escreve a profundidade global.
                directoryFile.write((char*)array, sizeof(unsigned int) * pow(2,hash.profundidade)); //Escreve o novo vetor do diretório.
                directoryFile.close();
                LinkDirectoryToIndex(cpf,address);      //Tenta inserir o bucjt novamente (chamada recursiva).
           }
       }
       else{
           /*
           * Caso o "is_linked" seja verdadeiro, é necessário fazer um rearroanjo dos ponteiros caso o endereço retornado seja diferente
           * de 1, pois uma nova página foi criada para inserir o novo bucket necessitando então de um rearranjo dos ponteiros do vetor
           * do diretório para comportar a nova página.
           */

          if(bucket.ponteiro != 1){
              RearrangePointers(bucket.cpf, nBits(bucket.cpf) + 1, bucket.ponteiro);
              array[bucket.cpf] = bucket.ponteiro;
              directoryFile.open(directoryFileName.c_str(), std::ios::out | std::ios::binary);
              directoryFile.write((char*)array, sizeof(unsigned int) * pow(2,hash.profundidade));
              directoryFile.close();
          }
       }
    }
}

Bucket GerenciadorArquivo::LinkIndexToMain(unsigned int pos, Bucket b){
    if(indexFile.is_open()){
        indexFile.close();
    }

    /*
    * cria uma página nova com tamanho máximo de buckets 
    */
    Pagina temp(hash.Buckets_MAX);
    Bucket returnBucket;

    /*
    * Aloca dinamicamente memória para receber os buckets 
    */
    temp.criarPagina(temp.pBcket_MAX);
    indexFile.open(indexFileName.c_str(), std::ios::in | std::ios::binary);

    if(indexFile.peek() == EOF){
        /*
        * Assim como na função LinkDirectoryToIndex, essa condição é exectuda uma única vez para 
        * cada sistema de arquivos novos.
        */

       indexFile.close();
       indexFile.open(indexFileName.c_str(), std::ios::out | std::ios::binary);
       returnBucket.ponteiro = indexFile.tellp();   //Armazena o endereço da página recem criada.
       returnBucket.cpf = 0;

       temp.b[temp.pBuckets] = b;                   //Insere o bucket na primeira posição.
       temp.pBuckets++;                             //Incrementa a quantidade de buckets e permite que um próximo buckt seja inserido.
       hash.pagina = temp;                          //Atribui a hash a página ser inserida.
       writePage(hash.pagina);                      //Escreve a página no arquivo

       temp.deletaPagina();                         //Deleta a página temporária criada para inserção.
       indexFile.close();

       hash.is_linked = true;                       //Muda a flag para true indicando que foi possível inserir com sucesso novo bucket.

       return returnBucket;                         //retorna um bucket com a flag. 
    }
    else{
        indexFile.close();
        indexFile.open(indexFileName.c_str(), std::ios::in | std::ios::binary);
        Pagina temp2;

        indexFile.seekg(pos, std::ios::beg);        //posiciona o ponteiro para a posição da página onde o bucket deve ser inserido.
        temp2 = readPage();                         //Lê a página.

        indexFile.close();
        indexFile.open(indexFileName.c_str(), std::ios::out | std::ios::in | std::ios::binary);

        /*
        * verifica se a página já esta cheia. Se não estiver, executa a condição.
        */
        if(temp2.pBuckets != temp2.pBcket_MAX){
            indexFile.seekp(pos, std::ios::beg);    //Posiciona o ponteiro para a posição da página onde o bucket deve ser escrito.
            returnBucket.ponteiro = 1;              //Indica que nenhuma página nova foi criada.
            returnBucket.cpf = 0;

            temp2.b[temp2.pBuckets] = b;            //Adiciona o bucket a página.
            temp2.pBuckets++;                       //Incrimenta o contador de buckets por página.
            hash.pagina = temp2;

            writePage(hash.pagina);                 //reescreve a página no arquivo de indices novamente.

            temp.deletaPagina();
            temp2.deletaPagina();
            indexFile.close();

            hash.is_linked = true;

            return returnBucket;
        }
        else{
            /*
            * Verifica se a profundidade da página atende a profundidade global da Hash.
            */
           if(temp2.pDepth = hash.profundidade){
               temp.deletaPagina();
               temp2.deletaPagina();
               indexFile.close();

               returnBucket.ponteiro = 1;           //Retirna 1 indicando que nenhuma página foi criada.
               returnBucket.cpf = 0;

               hash.is_linked = false;              //Mudaa flag para falso indicando que não foi possível inserir o bucket.

               return returnBucket;
           }
           /*
           * Caso a profundidade local da página não esteja no máximo definido pela Hash, logo linkDirectoryToIndex não precisa ser 
           * informada para que uma nova expansão ocorra. Sendo assim, uma nova página será criada e inserido o novo bucket possibilitando
           * o rearanjo dos ponteiros no diretório. 
           */
           else{
               Pagina temp3(hash.Buckets_MAX);
               temp3.criarPagina(temp3.pBcket_MAX);

               unsigned int key = hash.hashFunction(b.cpf);     //Recebe o valor da posição indicada pelo ponteiro para esse bucket. 
               unsigned int numberOfBits = nBits(key);          //Calcula quantos bits são necessários para representar a posição.

               int i;

               /*
               * A condição abaixo é necessaria, uma vez que em grandes escalas se a posição do diretório for 0 ou qualquer posição que já tenha
               * sido atribuida um valor de endereço de uma página criada anterioremente é necessário que o próximo ponteiro diretamente relacionado
               * a posição no vetor do diretório criado acima seja usado para referenciar a nova página.
               */

              if(key == 0 || !compareAddresses(key, key - (unsigned int) (pow(2,numberOfBits) / 2))){
                  if(key != 0){
                      i = numberOfBits + 1;
                  }
                  else{
                      i = numberOfBits;
                  }
                  
                  while(i <= hash.profundidade){
                      /*
                      * Procura o próximo ponteiro que se relaciona com o ponteiro Inicialmente calculado anteriormente
                      * e que não referência a mesma página.
                      */
                     if(compareAddresses(key, key + (unsigned int)(pow(2, i) / 2))){
                         key = key + (unsigned int)(pow(2, i) / 2);
                         break;
                     }
                     i++;
                  }

                  numberOfBits = nBits(key);                    //recalcula a nova quantidade d ebits.

                  for(i = 0; i < temp2.pBcket_MAX; i++){

                      /*
                      * Faz o rehash com a nova profundidade da página
                      */
                      if(temp2.b[i].cpf % (unsigned int)pow(2, temp2.pDepth + 1) == key ){
                          temp.b[temp.pBuckets] = temp2.b[i];   //transfere o buckt para a nova página caso o rehash possibilite.
                          temp2.pBuckets--;
                          temp.pBuckets++;
                      }
                      else{
                          
                      }
                  }
              }
           }
        }
    }



}