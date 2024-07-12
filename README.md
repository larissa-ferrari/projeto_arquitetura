# Otimização de Funções Criptográficas em OpenSSL Utilizando Assembly Inline e Intrinsics

## Alunos

Larissa Rodrigues Ferrari | RA: 221151214
Murilo Augusto Venturato | RA: 221153861

## Introdução

Este projeto integra a disciplina de Arquitetura e Organização de Computadores e visa otimizar trechos do código-fonte da biblioteca OpenSSL, utilizando assembly inline e intrinsics, com o objetivo de melhorar o desempenho das funções de hash SHA-256. A eficácia das otimizações será avaliada por meio de ferramentas de perfilamento e benchmarking, permitindo uma análise detalhada dos ganhos de desempenho obtidos.

## Sobre a OpenSSL

OpenSSL é uma robusta biblioteca de código aberto amplamente utilizada para implementar os protocolos SSL (Secure Sockets Layer) e TLS (Transport Layer Security). Ela fornece uma vasta gama de ferramentas para criptografia, garantindo a segurança de comunicações através da internet. Além de SSL/TLS, OpenSSL oferece diversas funcionalidades de criptografia, incluindo algoritmos de chave simétrica e assimétrica, funções de hash, e geração de números aleatórios.

### Utilização da OpenSSL

OpenSSL é essencial em muitos contextos, incluindo:

1. Segurança de Navegação na Web: OpenSSL é utilizado para habilitar conexões HTTPS, garantindo que os dados trocados entre navegadores e servidores web estejam seguros.
2. Serviços de E-mail: Muitos servidores de e-mail utilizam OpenSSL para proteger conexões SMTP, IMAP e POP3.
3. VPNs: Redes Privadas Virtuais (VPNs) frequentemente usam OpenSSL para proteger dados trafegados entre o cliente e o servidor.
4. Aplicações de Mensageria: Aplicações que requerem comunicação segura, como sistemas de mensagens instantâneas e aplicativos de colaboração, utilizam OpenSSL.
5. Assinatura e Verificação de Certificados: OpenSSL é usado para criar e gerenciar certificados digitais, fundamentais para a infraestrutura de chave pública (PKI).

### Função Focada na Otimização

O foco deste projeto é otimizar as funções de hash SHA-256 da OpenSSL. SHA-256 (Secure Hash Algorithm 256-bit) é uma função de hash criptográfica que gera um valor de hash de 256 bits (32 bytes) para uma entrada fornecida. É amplamente utilizada para garantir a integridade e autenticidade dos dados em diversas aplicações de segurança.

A otimização dessas funções é crucial, pois elas são intensivamente utilizadas em várias operações criptográficas, e melhorias de desempenho nessas funções podem resultar em ganhos significativos em aplicações que dependem de operações de hash rápidas e eficientes.

## Compilando OpenSSL

Para otimizar e realizar o profiling das funções de hash SHA-256 na biblioteca OpenSSL, é necessário compilar o OpenSSL a partir do código-fonte. Aqui estão as instruções detalhadas para baixar, configurar e compilar o OpenSSL.

**1. Baixar o Source Code do OpenSSL:**

Primeiro, baixe o código-fonte ou clone o repositório do GitHub:

`$ git clone https://github.com/openssl/openssl.git`

**2. Navegar para o Diretório do Source Code:**

Depois de clonar o repositório, navegue até o diretório do código-fonte:

`$ cd openssl`

**3. Configurar o Ambiente:**

Configure o ambiente para desabilitar otimizações e habilitar opções de depuração:

`$ ./config no-asm -d`

-   `no-asm` desabilita as otimizações de assembly.
-   `-d` habilita opções de depuração.

**4. Compilar o OpenSSL:**

Compile o OpenSSL usando apenas dois núcleos:

`bash $ make -j2`

Ao compilar o OpenSSL com essas configurações, você desabilitará as otimizações do compilador e ativará opções de depuração, facilitando o uso de ferramentas de profiling como Valgrind e perf.

Se precisar de mais configurações específicas ou tiver outras necessidades, consulte a documentação oficial do OpenSSL ou as opções de configuração disponíveis executando:

`bash $ ./config --help`

Isso fornecerá uma visão geral das opções adicionais que podem ser usadas durante a configuração e compilação do OpenSSL.

## Download das Ferramentas de Perfilamento

Para realizar o profiling do OpenSSL, utilizaremos as ferramentas perf e Valgrind. Ambas podem ser baixadas diretamente dos repositórios da sua distribuição Linux.

### Passos para Instalar perf e Valgrind

1. Atualizar os Repositórios do Sistema:

    `bash $ sudo apt update`

2. Instalar perf e Valgrind:

    `$ sudo apt install linux-tools-common linux-tools-generic linux-tools-$(uname -r) valgrind`

    - `linux-tools-common`: Pacotes comuns para ferramentas Linux.
    - `linux-tools-generic`: Pacotes genéricos para ferramentas Linux.
    - `linux-tools-$(uname -r)`: Ferramentas específicas para a versão do kernel em execução.
    - `valgrind`: Ferramenta de profiling e debugging.

#### Verificação da Instalação

Depois de instalar, você pode verificar se as ferramentas foram instaladas corretamente executando os seguintes comandos no terminal:

Para perf:
`$ perf --version`

Para valgrind:
`$ valgrind --version`

Esses comandos devem retornar a versão instalada de cada ferramenta, confirmando que a instalação foi bem-sucedida.

## Profiling Detalhado da OpenSSL

**1. Executar o perf record:**

Primeiro, gravamos os dados de profiling enquanto executamos uma operação com OpenSSL:

`$ sudo perf record -o openssl.perf.data ./main`

**2. Analisando os Dados do perf record:**

Depois de executar o perf record, analisamos os dados gravados usando o perf report:

`$ sudo perf report -i openssl.perf.data > report_openssl.txt`

Isso gerou um relatório detalhado com informações sobre o uso da CPU, tempo gasto em funções específicas da OpenSSL, chamadas de sistema, e outras métricas de desempenho relevantes.

O relatório detalhado do perf nos fornece informações cruciais sobre onde o tempo está sendo consumido durante a execução do programa. Aqui estão os pontos principais baseados nas porcentagens de overhead por função do kernel:

-   cgroup_rstat_updated (62,51%): Esta função do kernel representa a maior parte do overhead. Isso sugere que uma porção significativa do tempo de execução está sendo gasto em operações relacionadas às estatísticas de recursos do cgroup. Otimizar o uso ou a configuração de cgroups pode ajudar a reduzir esse overhead.

-   do_fault (35,20%): Responsável por uma grande parte do overhead, do_fault indica atividades relacionadas a falhas de página. O número de page faults observado pode estar relacionado a isso. Investigar e mitigar as causas das falhas de página pode ajudar a melhorar o desempenho.

-   Outras funções do kernel: Funções como \_raw_spin_lock, native_apic_msr_write, e native_write_msr contribuem com uma pequena porcentagem do overhead total, mas ainda assim podem ser otimizadas dependendo do contexto e da frequência com que são chamadas durante a execução do programa.

## Objetivo do Projeto

Nosso objetivo principal consistia em compilar a biblioteca OpenSSL, criar um código que utilizasse a função SHA-256 proveniente dela, otimizar essa função utilizando assembly inline e intrinsics, substituir na biblioteca OpenSSL a função SHA-256 original pela otimizada, recompilar e então realizar um benchmarking para comparar o desempenho da função original com a otimizada.

Inicialmente, focamos nossos esforços em elaborar códigos mais simples que utilizassem a função SHA-256 da biblioteca OpenSSL. Após isso, otimizamos esses códigos e comparamos o desempenho das versões original e otimizada. Dessa forma, pudemos identificar os ganhos de desempenho proporcionados pelas otimizações realizadas, antes de aplicar essas melhorias diretamente na biblioteca OpenSSL.

## Estrutura do Projeto

O projeto foi organizado em pastas e arquivos de acordo com a seguinte estrutura:

-   `openssl/`: Contém o código-fonte da biblioteca OpenSSL.
-   `optimization-1/`: Contém o código-fonte e os arquivos relacionados à primeira otimização realizada.
-   `optimization-2/`: Contém o código-fonte e os arquivos relacionados à segunda otimização realizada.
-   `optimization-3/`: Contém o código-fonte e os arquivos relacionados à terceira otimização realizada.

Cada pasta de otimização apresenta três arquivos principais:

-   `main.c`: Código original utilizando a função SHA-256 da biblioteca OpenSSL.
-   `main_optimized.c`: Código otimizado utilizando assembly inline ou intrinsics para melhorar o desempenho da função SHA-256.
-   `report_main.txt`: Relatório de profiling e benchmarking do código original (`main.c`), gerado utilizando ferramentas como Valgrind e perf.
-   `report_main_optimized.txt`: Relatório de profiling e benchmarking do código otimizado (`main_optimized.c`), gerado utilizando ferramentas como Valgrind e perf.

## Tentativas de Otimização

### Otimização 1: Otimização do Código SHA-256 Utilizando Intrinsics Intel (SSE/AVX)

Em uma primeira abordagem mais simples, focamos em otimizar o código simplificado ao invés da função dentro da biblioteca OpenSSL. A otimização no código de hash SHA-256 utilizando intrinsics Intel foi realizada para acelerar o processo de cálculo do hash através do uso de operações SIMD (Single Instruction, Multiple Data). As intrinsics são funções que permitem o acesso direto a instruções específicas do conjunto de instruções do processador, como as SSE (Streaming SIMD Extensions) e AVX (Advanced Vector Extensions).

#### Otimizações Específicas Realizadas

1. **Carregamento de Dados em Blocos de 128 Bits**
   Em vez de processar a string de entrada byte a byte, o código otimizado carrega os dados em blocos de 128 bits (16 bytes) usando a função `_mm_loadu_si128`. Isso é feito dentro do loop que percorre a entrada em pedaços de 64 bytes:

    ```c
    __m128i block[4];
    for (int i = 0; i < 4; i++) {
        block[i] = _mm_loadu_si128((__m128i*)(data + i * 16));
    }
    ```

2. **Uso de Registradores SIMD para Armazenar o Estado do Hash**
   O estado interno do hash SHA-256 é armazenado em registradores SIMD de 128 bits:

    ```c
    __m128i state[8];
    for (int i = 0; i < 8; i++) {
        state[i] = _mm_loadu_si128((__m128i*)&sha256.h[i * 4]);
    }
    ```

3. **Execução de Instruções SHA-256 Específicas**
   As instruções intrinsics `_mm_sha256rnds2_epu32` são usadas para realizar as operações de compressão do SHA-256. Essas instruções são específicas para acelerar a computação dos valores intermediários do hash:

    ```c
    for (int i = 0; i < 64; i += 16) {
        state[0] = _mm_sha256rnds2_epu32(state[0], state[4], block[i / 16]);
        state[1] = _mm_sha256rnds2_epu32(state[1], state[5], block[i / 16]);
        state[2] = _mm_sha256rnds2_epu32(state[2], state[6], block[i / 16]);
        state[3] = _mm_sha256rnds2_epu32(state[3], state[7], block[i / 16]);
    }
    ```

4. **Armazenamento do Estado do Hash de Volta na Estrutura Original**
   Após processar os blocos de dados, o estado atualizado do hash é armazenado de volta na estrutura original do SHA-256:

    ```c
    for (int i = 0; i < 8; i++) {
        _mm_storeu_si128((__m128i*)&sha256.h[i * 4], state[i]);
    }
    ```

#### Resultados e Impacto

Essas otimizações permitem que o cálculo do hash seja realizado de maneira mais eficiente ao explorar o paralelismo disponível nos registradores SIMD do processador, o que pode resultar em uma melhora significativa no desempenho para grandes volumes de dados. No entanto, é importante notar que o uso de intrinsics pode tornar o código menos portátil, pois essas instruções são específicas para processadores Intel que suportam essas extensões.

Por meio dos relatórios gerados de profiling, chegamos ao código que melhor atingiu otimização de desempenho:

#### Observações sobre as Métricas

1. **Tempo de Clock e Tempo Decorrido**:

    - O tempo de clock aumentou de 0,81 msec para 1,36 msec, o que pode indicar um overhead devido ao uso de intrinsics.
    - No entanto, o tempo decorrido total diminuiu significativamente de 0,695060320 segundos para 0,002857384 segundos. Essa é uma melhoria significativa e sugere que a otimização foi bem-sucedida em reduzir o tempo total de execução do programa.

2. **Ciclos e Instruções**:

    - O número de ciclos diminuiu de 2.640.405 para 2.565.617, indicando que o programa otimizado requer menos ciclos de CPU para concluir a mesma tarefa.
    - O número de instruções também diminuiu ligeiramente de 2.683.364 para 2.673.458, mostrando uma pequena eficiência na redução do total de instruções executadas.

3. **Instruções por Ciclo (IPC)**:

    - A IPC aumentou de 1,02 para 1,04, o que sugere que o programa otimizado está fazendo melhor uso dos ciclos de CPU disponíveis, executando mais instruções por ciclo.

4. **Branches e Branch-misses**:
    - O número de branches e branch-misses são quase os mesmos, com uma leve variação. Isso indica que a lógica de controle do programa não foi significativamente alterada, mas a leve variação nos branch-misses (+50) não deve impactar significativamente o desempenho geral.

#### Conclusão

Em resumo, a principal melhoria foi a significativa redução no tempo total de execução do programa, evidenciando que a otimização é eficaz em acelerar o cálculo do hash SHA-256 através do uso eficiente dos recursos do processador.

### Otimização 2: Otimização do Código SHA-256 Utilizando Intrinsics Intel (SHA-NI)

A segunda otimização foi baseada na primeira, com foco em melhorar o desempenho do código de hash SHA-256 utilizando intrinsics Intel para acelerar o processo de cálculo do hash através de operações SIMD (Single Instruction, Multiple Data).

#### Melhorias Realizadas

**Uso de Intrinsics SHA-NI**:

-   As intrinsics `_mm_loadu_si128` e `_mm_sha256rnds2_epu32` foram utilizadas para aproveitar a extensão SHA-NI disponível em processadores Intel modernos, permitindo um processamento de bloco de dados mais eficiente e paralelo.

**Processamento SIMD**:

-   A utilização de `__m128i` permitiu carregar e manipular dados em paralelo, processando quatro blocos de dados simultaneamente e realizando operações de mistura com `_mm_sha256rnds2_epu32`.

**Loop Unrolling Implícito**:

-   A estrutura do código, onde quatro blocos são processados em cada iteração do loop externo, efetivamente realiza um tipo de loop unrolling, reduzindo a sobrecarga de controle de loop e melhorando o desempenho.

**Acesso Direto à Memória**:

-   O uso de `_mm_loadu_si128` e `_mm_storeu_si128` permitiu acesso direto à memória, sem necessidade de alinhamento específico, mantendo a eficiência e a integridade dos dados.

#### Considerações

**Compatibilidade do Hardware**:

-   A otimização depende do suporte às instruções SHA-NI no hardware. Processadores modernos da Intel geralmente suportam essas instruções, mas é necessário verificar a compatibilidade.

**Otimizações de Compilação**:

-   Opções de compilação como `-march=native -msha` foram usadas para habilitar suporte a instruções específicas do processador.

#### Análise das Métricas

Comparando os relatórios, podemos observar:

-   **Tempo de Clock**: O tempo de clock aumentou de 0,81 msec para 0,87 msec.
-   **Ciclos**: O número de ciclos aumentou de 2.640.405 para 2.948.494.
-   **Instruções por Ciclo (IPC)**: A IPC diminuiu de 1,02 para 0,92.
-   **Tempo Decorrido**: O tempo decorrido total aumentou de 0,695060320 segundos para 0,747210389 segundos.

Esses resultados mostram que, embora a intenção da otimização com intrinsics tenha sido melhorar o desempenho, os ganhos não foram alcançados conforme esperado. O aumento nos ciclos e no tempo de clock indica que a implementação com intrinsics pode ter introduzido algum overhead adicional, possivelmente devido à complexidade do uso de intrinsics e a falta de otimização completa.

### Otimização 3: Compilação da Biblioteca OpenSSL

Na terceira otimização, a biblioteca OpenSSL foi compilada dentro do próprio projeto e apontada por meio do comando:

`gcc -o main main.c -I./openssl/include -L./openssl/lib -lssl -lcrypto -mavx2`

`gcc -o main_optimized main_optimized.c -I./openssl/include -L./openssl/lib -lssl -lcrypto -mavx2`

Usando essa versão compilada da biblioteca, algumas funções utilizadas anteriormente nos códigos `main.c` foram avisadas como depreciadas (`SHA256_Init()`, `SHA256_Update()`, `SHA256_Final()`), então alteramos o código e tentamos outra abordagem de otimização nas funções que compõem a geração do SHA-256.

#### Uso de Intrinsics AVX2

**Instruções AVX2 (`__m256i`)**:

-   Permitem operar em blocos de dados de 256 bits, ou 8 inteiros de 32 bits, ao mesmo tempo.

**Função de rotação à direita**:

-   A função `rotr` foi implementada usando intrinsics para realizar a operação de rotação à direita (uma operação comum no SHA-256) de forma eficiente.

```c
static inline __m256i rotr(__m256i x, int n) {
    return _mm256_or_si256(_mm256_srli_epi32(x, n), _mm256_slli_epi32(x, 32 - n));
}
```

**Transformação SHA-256 com intrinsics**:

-   A função `sha256_transform_intrinsics` realiza uma rodada do algoritmo SHA-256 utilizando operações vetoriais.
-   As constantes K são carregadas e utilizadas diretamente em operações vetoriais.
-   O array W é preenchido e atualizado usando operações AVX2, aproveitando a paralelização.
-   As principais operações do SHA-256 (como Ch, Maj, Σ0, Σ1, etc.) são realizadas utilizando intrinsics, permitindo que múltiplos cálculos sejam feitos simultaneamente.

```c
void sha256_transform_intrinsics(__m256i state[8], const unsigned char data[64]) {
    // Carregar os dados e preparar o array W
    for (int i = 0; i < 16; i++) {
        W[i] = _mm256_set1_epi32(((unsigned int*)data)[i]);
    }

    // Calcular W[16] até W[63]
    for (int i = 16; i < 64; i++) {
        __m256i s0 = _mm256_xor_si256(rotr(W[i-15], 7), rotr(W[i-15], 18));
        s0 = _mm256_xor_si256(s0, _mm256_srli_epi32(W[i-15], 3));
        __m256i s1 = _mm256_xor_si256(rotr(W[i-2], 17), rotr(W[i-2], 19));
        s1 = _mm256_xor_si256(s1, _mm256_srli_epi32(W[i-2], 10));
        W[i] = _mm256_add_epi32(W[i-16], s0);
        W[i] = _mm256_add_epi32(W[i], W[i-7]);
        W[i] = _mm256_add_epi32(W[i], s1);
    }

    // Inicializar variáveis de estado
    __m256i a = state[0];
    __m256i b = state[1];
    __m256i c = state[2];
    __m256i d = state[3];
    __m256i e = state[4];
    __m256i f = state[5];
    __m256i g = state[6];
    __m256i h = state[7];

    // Rodadas do SHA-256
    for (int i = 0; i < 64; i++) {
        __m256i S1 = _mm256_xor_si256(rotr(e, 6), rotr(e, 11));
        S1 = _mm256_xor_si256(S1, rotr(e, 25));
        __m256i ch = _mm256_xor_si256(_mm256_and_si256(e, f), _mm256_andnot_si256(e, g));
        __m256i temp1 = _mm256_add_epi32(h, S1);
        temp1 = _mm256_add_epi32(temp1, ch);
        temp1 = _mm256_add_epi32(temp1, _mm256_set1_epi32(K[i]));
        temp1 = _mm256_add_epi32(temp1, W[i]);
        __m256i S0 = _mm256_xor_si256(rotr(a, 2), rotr(a, 13));
        S0 = _mm256_xor_si256(S0, rotr(a, 22));
        __m256i maj = _mm256_xor_si256(_mm256_and_si256(a, b), _mm256_and_si256(a, c));
        maj = _mm256_xor_si256(maj, _mm256_and_si256(b, c));
        __m256i temp2 = _mm256_add_epi32(S0, maj);
        h = g;
        g = f;
        f = e;
        e = _mm256_add_epi32(d, temp1);
        d = c;
        c = b;
        b = a;
        a = _mm256_add_epi32(temp1, temp2);
    }

    // Atualizar estado
    state[0] = _mm256_add_epi32(state[0], a);
    state[1] = _mm256_add_epi32(state[1], b);
    state[2] = _mm256_add_epi32(state[2], c);
    state[3] = _mm256_add_epi32(state[3], d);
    state[4] = _mm256_add_epi32(state[4], e);
    state[5] = _mm256_add_epi32(state[5], f);
    state[6] = _mm256_add_epi32(state[6], g);
    state[7] = _mm256_add_epi32(state[7], h);
}
```

#### Impacto das Otimizações

**Desempenho**:

-   O uso de intrinsics e operações vetoriais permite que várias operações matemáticas sejam realizadas simultaneamente, melhorando significativamente o desempenho em comparação com a implementação tradicional.

**Paralelismo**:

-   As operações vetoriais permitem processar múltiplos elementos de dados em paralelo, aproveitando ao máximo a largura de banda da CPU.

#### Resultados de Performance

A otimização com intrinsics AVX2 resultou em melhorias significativas no desempenho do código SHA-256. O tempo de clock foi reduzido de 3,49 msec para 3,08 msec, indicando uma execução mais rápida. Além disso, houve uma redução considerável nos ciclos necessários, de 11.715.410 para 10.093.874, o que demonstra uma maior eficiência na utilização da CPU. O IPC (Instruções por Ciclo) aumentou de 0,83 para 0,97, refletindo uma execução mais eficiente das instruções. As branches permaneceram praticamente inalteradas, enquanto o número de branch misses teve uma leve redução, de 37.251 para 36.595, melhorando a previsibilidade do fluxo de controle. No entanto, a otimização não resultou em uma diminuição significativa no número total de instruções executadas, sugerindo que a lógica geral do código não mudou drasticamente. Além disso, houve um leve aumento nas page-faults e na quantidade de tempo total decorrido, o que pode indicar uma sobrecarga adicional em algumas áreas do sistema.

### Otimização 4: Hipoóteses de Otimização

A função sha256_block_data_order pode ser encontrada no arquivo crypto/sha/sha256.c dentro da estrutura de arquivos da biblioteca OpenSSL. Esta função é fundamental para o desempenho global da função de hash SHA-256, pois define como os dados são manipulados e processados em blocos individuais para calcular o hash.

Nesta fase do projeto, foram exploradas diversas hipóteses de otimização para a função sha256_block_data_order. Essas hipóteses foram fundamentadas em conhecimentos adquiridos durante o desenvolvimento do projeto e em pesquisas extensas sobre otimizações de funções criptográficas. O objetivo principal era substituir ou melhorar significativamente a implementação existente da função sha256_block_data_order, visando assim aprimorar o desempenho geral da função de hash SHA-256 dentro da biblioteca.

No entanto, devido à complexidade dos testes requeridos pela biblioteca OpenSSL, à limitação de realizar testes em uma arquitetura adequada (os testes foram conduzidos no WSL2) e à restrição de tempo, não foi possível implementar e testar as hipóteses de otimização conforme planejado. A pasta dedicada a essa etapa do projeto contém as hipóteses detalhadas e as implementações propostas, que abrangem desde melhorias na utilização de instruções SIMD específicas até otimizações algorítmicas para aproveitar melhor as características dos processadores modernos.

As implementações propostas foram estruturadas com base em técnicas como o uso de intrinsics SIMD avançados, otimizações de loops e ajustes na manipulação de dados, todas com o objetivo de maximizar o paralelismo e minimizar o tempo de execução das operações de hash SHA-256. Essas hipóteses representam um potencial significativo para futuras iterações do projeto, onde a implementação e testes detalhados poderiam validar sua eficácia na melhoria do desempenho da função criptográfica.
