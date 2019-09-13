#include <graphics.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>
#include <MMSystem.h>

//MODELO POSSÍVEL PARA O ASTERÓIDE
typedef struct {
    char src[30];
    int resistencia;
    int pontuacao;
    int w; // LARGURA
    int h; // ALTURA
    float f; // Fator de velocidade
} model;

//TODO ASTEROIDE TEM
typedef struct {
    int x;
    int y;
    model m;
} asteroide;


//ESTRUTURA DO LASER
typedef struct {
    int x;
    int y;
} laser;

//ESTRUTURA DA NAVE
typedef struct {
    char src[30];
    int x;
    int y;
} nave;

//ESTRUTURA DO RELATÓRIO
typedef struct{
    int astDestruidos;
    int lasersSoltos;
    float tempoPartida;
}relatorio;

//NÓ PARA RELATÓRIO
struct noRelat{
    relatorio r;
    struct noRelat *prox;
};

//FILA PARA O RELATÓRIO
typedef struct{
    struct noRelat *inicio;
    struct noRelat *fim;
}filaRelat;

//NÓ DO ASTEROIDE
struct noAsteroide {
    struct noAsteroide *ant;
    asteroide a;
    struct noAsteroide *prox;
};

//LISTA PARA ASTEROIDE
typedef struct
{
    struct noAsteroide *inicio;
    struct noAsteroide *fim;
} listalineardeAst;

//NÓ DO LASER
struct noLaser {
    struct noLaser *ant;
    laser l;
    struct noLaser *prox;
};

//LISTA PARA LASER
typedef struct
{
    struct noLaser *inicio;
    struct noLaser *fim;
} listalineardeLaser;

//FUNÇÕES PARA FILA DO RELATÓRIO
void createFr(filaRelat *f);
int  isEmpty(filaRelat f);
int  insertFr(filaRelat *f, relatorio r);
int  removeFr(filaRelat *f, relatorio *r);

//FUNÇÕES PARA OS ASTEROIDES
void initModel(model *a, char *src, int resistencia, int pontuacao, int w, int h, float f);
void criaListaAsteroide(listalineardeAst *q);
int  insereListaAsteroide(listalineardeAst *q, asteroide a);
void mostraAsteroides(listalineardeAst *q);
void clearAsteroides(listalineardeAst *q);
int removeeAsteroide(listalineardeAst *q, struct noAsteroide *ast);

//FUNÇÕES PARA O LASER
void criaListaLaser(listalineardeLaser *q);
int  insereListaLaser(listalineardeLaser *q, laser l, relatorio *r);
void mostraLasers(listalineardeLaser *q);
void clearLasers(listalineardeLaser *q);
int removeeLaser(listalineardeLaser *q, struct noLaser *laser);

//FUNÇÕES GERAIS
void verificaColisao(listalineardeAst *lAst, listalineardeLaser *lLaser, relatorio *r);
void clear_keyboard_buffer(void);

//VARIÁVEIS GLOBAIS
int speed = 8, vidas = 3, pontuacao = 0;

main()
{
    //VARIÁVEIS CRIADAS
    int i, ultimo_ast, ultimo_laser, r;
    int gdriver = EGA, gmode = EGAHI; // MODO GRAFICO PARA SUPORTE DE MULTIPLAS PAGINAS
    char in, exitflag, pontuacao_aux[30], *trash;
    model models[6];
    relatorio relat, aux_relat;
    nave n = {"assets/ship.gif",630,580};
    asteroide ast;
    asteroide novoAst;
    laser l;
    listalineardeAst listaAsteroide;
    listalineardeLaser listaLaser;
    filaRelat filaR;
    time_t t_ini, t_fim;

    //DEFININDO OS MODELOS DOS ASTEROIDES
    initModel(&models[0],"assets/asteroide1.gif",3, 1, 60, 60, 1);
    initModel(&models[1],"assets/asteroide2.gif",6, 3, 85, 75, 0.8);
    initModel(&models[2],"assets/asteroide3.gif",9, 6, 100, 85, 0.7);
    initModel(&models[3],"assets/asteroide4.gif",12, 10, 110, 104, 0.5);
    initModel(&models[4],"assets/asteroide5.gif",15, 15, 110, 107, 0.3);
    initModel(&models[5],"assets/asteroide6.gif",25, 25, 180, 173, 0.1);

    //CRIANDO FILA PARA RELATÓRIO
    createFr(&filaR);

    //CRIANDO AS LISTAS
    criaListaAsteroide(&listaAsteroide);
    criaListaLaser(&listaLaser);

    initgraph(&gdriver, &gmode, "");
    initwindow(1360,730);

    //WHILE PARA VERIFICAR QUANDO JOGADOR QUER SAIR
    while (exitflag != 'e') {
        exitflag = ' ';

        //INICIANDO BASE DO JOGO
        PlaySound(TEXT("assets/background_menu.wav"), NULL, SND_ASYNC);
        clearAsteroides(&listaAsteroide);
        clearLasers(&listaLaser);

        readimagefile("assets/background.jpg",0,0,1360,730);
        readimagefile("assets/title.gif",280,200,1080,422);
        readimagefile("assets/presstocontinue.gif",480,440,880,520);
        readimagefile("assets/dir.gif",1120,570,1320,680);
        swapbuffers();

        //ESPERA DIGITAR UMA TECLA PARA INICIAR PARTIDA
        in = '.';
        do {
            if (kbhit())
                in = getch();
        } while (in != ' ');

        //PARTIDA INICIADA
        PlaySound(TEXT("assets/start.wav"), NULL, SND_ASYNC);
        relat.astDestruidos = 0;
        relat.lasersSoltos = 0;
        relat.tempoPartida = 0;
        t_ini = time(NULL);

        //GERA PRIMEIRO ASTEROIDE
        srand(clock());
        ast.m = models[rand()%6];
        ast.x = rand()%1160 + 100;
        ast.y = 0;
        insereListaAsteroide(&listaAsteroide, ast);
        ultimo_ast = time(NULL);
        ultimo_laser = time(NULL);

        vidas = 3; pontuacao = 0;
        //WHILE PARA VERIFICAR SE TA VIVO
        while(vidas > 0) {
            swapbuffers();

            readimagefile("assets/background.jpg",0,0,1360,730);

            //VERIFICA SUA VIDA
            if (vidas == 3) readimagefile("assets/earth_3.gif",0,0,120,120);
            if (vidas == 2) readimagefile("assets/earth_2.gif",0,0,120,120);
            if (vidas == 1) readimagefile("assets/earth_1.gif",0,0,120,120);

            //MOSTRA PONTUAÇÃO
            sprintf(pontuacao_aux,"%03d",pontuacao);

            settextstyle(3, HORIZ_DIR, 2);
            outtextxy(1250,20,"SCORE");
            settextstyle(3, HORIZ_DIR, 6);
            outtextxy(1245,45,pontuacao_aux);

            //DIMINUI POSIÇÃO DO ASTEROIDE
            ast.y += 2; // #1

            // VERIFICA SE A DIFERENÇA DE TEMPO ENTRE O ÚLTIMO ASTEROIDE E AGORA É MAIOR QUE 6 SEG
            if (time(NULL) - ultimo_ast > 6) {
                ultimo_ast = time(NULL); // ATUALIZA O TEMPO DO ÚLTIMO ASTEROIDE PARA AGORA

                r = rand()%6;
                novoAst.m = models[r];
                novoAst.x = rand()%1160 + 100;
                novoAst.y = 0;
                insereListaAsteroide(&listaAsteroide, novoAst);
            }

            //FUNÇÃO QUE EXIBE OS ASTEROIDES
            mostraAsteroides(&listaAsteroide);

            //EXIBE A NAVE
            readimagefile(n.src,n.x,n.y,n.x+100,n.y+140); // DESENHA A NAVE

            //VERIFICA SEMPRE SE UM LASER ESTÁ COLIDINDO COM UM ASTEROIDE
            verificaColisao(&listaAsteroide,&listaLaser, &relat);

            //LÊ TECLA DO USUÁRIO
            while (kbhit()) {
                in = getch();
            }

            //VERIFICA TECLA
            switch(in) {
                //ESQUERDA
                case 'a':
                case 'A':
                    if (n.x>0)
                        n.x-=50;
                    break;
                //DIREITA
                case 'd':
                case 'D':
                    if (n.x<1260)
                        n.x+=50;
                    break;
                //LASER
                case 'l':
                case 'L':
                    //IMPEDE O JOGADOR DE FICAR SOLTANDO LASERS O TEMPO TODO
                    if (time(NULL) - ultimo_laser > 0.25) {
                        ultimo_laser = time(NULL);
                        l.x=n.x;
                        l.y=n.y;
                        insereListaLaser(&listaLaser,l, &relat );
                        PlaySound(TEXT("assets/blaster.wav"), NULL, SND_ASYNC);
                        break;
                    }
            }

            in = ' ';
        }

        //INICIA A TELA DE FIM DO JOGO
        PlaySound(TEXT("assets/ending.wav"), NULL, SND_ASYNC);
        readimagefile("assets/background.jpg",0,0,1360,730);
        readimagefile("assets/earth_0.gif",380,110,980,710);
        readimagefile("assets/presstoexit.gif",380,10,980,185);
        settextstyle(3, HORIZ_DIR, 6);
        outtextxy(577,340,"SCORE");
        settextstyle(3, HORIZ_DIR, 6);
        outtextxy(613,388,pontuacao_aux);

        //VERIFICA O TEMPO GASTO NA PARTIDA
        t_fim = time(NULL);
        relat.tempoPartida = difftime(t_fim, t_ini);

        //INSERE UM DADO NO RELATÓRIO
        insertFr(&filaR, relat);
        swapbuffers();

        //VERIFICA SE ELE QUER OU NÃO JOGAR DE NOVO
        do {
            if (kbhit())
                exitflag = getch();
        } while (exitflag!='c' && exitflag!='e');

        PlaySound(TEXT("assets/start.wav"), NULL, SND_ASYNC);
    }
    i=0;
    //EXIBE O RELATÓRIO AO FIM DA PARTIDA
    while(!isEmpty(filaR))
    {
        i++;
        removeFr(&filaR,&aux_relat);
        printf("----------------------------------------Relatorio----------------------------------------");
        printf("\nPartida: %d\nLasers soltos na partida: %d\nAsteroides destruidos: %d"
               "\nTempo Gasto: %.0fs\n\n\n", i, aux_relat.lasersSoltos, aux_relat.astDestruidos, aux_relat.tempoPartida);
    }
    closegraph();
    system("pause");
}

//FUNÇÃO PARA INICIAR MODELO DO ASTEROIDE
void initModel(model *a, char *src, int resistencia, int pontuacao, int w, int h, float f) {
    strcpy(a->src,src);
    a->resistencia = resistencia;
    a->pontuacao = pontuacao;
    a->w = w;
    a->h = h;
    a->f = f;
}

//FUNÇÃO PARA CRIAR LISTA DOS ASTEROIDES
void criaListaAsteroide(listalineardeAst *q)
{
    q->inicio = NULL;
    q->fim = NULL;
}

//FUNÇÃO PARA INSERIR OS ASTEROIDES NA LISTA
int  insereListaAsteroide(listalineardeAst *q, asteroide a)
{
    struct noAsteroide *aux;
    aux = (struct noAsteroide*) malloc(sizeof(struct noAsteroide));
    if (aux == NULL)
        return 0;

    if (q->inicio == NULL)
    {
        aux->a = a;
        aux->prox = NULL;
        aux->ant = NULL;
        q->inicio = aux;
        q->fim = aux;
        return 1;
    }

    aux->a = a;
    aux->prox = NULL;
    q->fim->prox = aux;
    aux->ant = q->fim;
    q->fim = aux;
    return 1;
}

//FUNÇÃO PARA EXIBIR OS ASTEROIDES
void mostraAsteroides(listalineardeAst *q){
    struct noAsteroide *aux, *aux2;
    aux = q->inicio;

    //EXIBIR TUDO O QUE TEM NA LISTA
    while(aux!=NULL){
        //SE O ASTEROIDE ATRAVESSAR, PERDE UMA VIDA, É REMOVIDO E EXIBE SOM DE REMOÇÃO
        if (aux->a.y >= getmaxy()) {
            vidas--;
            aux2 = aux->prox;
            removeeAsteroide(q, aux);
            aux = aux2;
            PlaySound(TEXT("assets/life_loss.wav"), NULL, SND_ASYNC);
        } else {
            //EXIBE ASTEROIDE E PARA CADA MODELO, UMA VELOCIDADE DE QUEDA É IMPOSTA
            aux->a.y += aux->a.m.f * speed + 1;
            readimagefile(aux->a.m.src,aux->a.x,aux->a.y,aux->a.x+aux->a.m.w,aux->a.y+aux->a.m.h);
            aux = aux->prox;
        }
    }
}

//FUNÇÃO PARA REMOVER TODOS OS ASTEROIDES DA LISTA
void clearAsteroides(listalineardeAst *q){
    struct noAsteroide *aux, *aux2;
    aux = q->inicio;
    while(aux!=NULL){
        aux2 = aux->prox;
        removeeAsteroide(q, aux);
        aux = aux2;
    }
}

//FUNÇÃO PARA REMOVER UM ASTEOIDE
int removeeAsteroide(listalineardeAst *q, struct noAsteroide *ast) {
    struct noAsteroide *aux, *atual, *frente;
    if(q->inicio==NULL){
        return 0;
    }
    if(q->inicio == ast){
        if(q->inicio == q->fim){
            aux = q->inicio;
            q->inicio = NULL; q->fim = NULL;
            free(aux);
            return 1;
        }
        else{
            aux = q->inicio;
            q->inicio = q->inicio->prox;
            q->inicio->ant = NULL;
            free(aux);
            return 1;
        }
    }
    if(q->fim == ast){
        aux = q->fim;
        q->fim = q->fim->ant;
        q->fim->prox = NULL;
        free(aux);
        return 1;
    }
    atual = q->inicio; frente = q->inicio->prox;
    while(atual != NULL){
        if(atual != ast){
            atual = frente;
            frente = frente->prox;
        }
        else break;
    }
    if(atual == q->fim){
        return 0;
    }
    aux = atual;
    atual->ant->prox = frente;
    frente->ant = atual->ant;
    free(aux);
    return 1;
}

//FUNÇÃO PARA CRIAR LISTA DE LASERS
void criaListaLaser(listalineardeLaser *q)
{
    q->inicio = NULL;
    q->fim = NULL;
}

//FUNÇÃO PARA INSERIR LASER NA LISTA
int  insereListaLaser(listalineardeLaser *q, laser l, relatorio *r)
{
    struct noLaser *aux;
    aux = (struct noLaser*) malloc(sizeof(struct noLaser));
    if (aux == NULL)
        return 0;

    if (q->inicio == NULL)
    {
        aux->l = l;
        aux->prox = NULL;
        aux->ant = NULL;
        q->inicio = aux;
        q->fim = aux;
        r->lasersSoltos++;
        return 1;
    }

    aux->l = l;
    aux->prox = NULL;
    q->fim->prox = aux;
    aux->ant = q->fim;
    q->fim = aux;
    //ACRESCENTA UM LASER NO RELATÓRIO DA PARTIDA
    r->lasersSoltos++;
    return 1;
}

//FUNÇÃO PARA REMOVER TODOS OS LASERS DA LISTA
void clearLasers(listalineardeLaser *q){
    struct noLaser *aux, *aux2;
    aux = q->inicio;
    while(aux!=NULL){
        aux2 = aux->prox;
        removeeLaser(q,aux);
        aux = aux2;
    }
}

//FUNÇÃO PARA REMOVER UM LASER DA LISTA
int removeeLaser(listalineardeLaser *q, struct noLaser *laser) {
    struct noLaser *aux, *atual, *frente;
    if(q->inicio==NULL){
        return 0;
    }
    if(q->inicio == laser){
        if(q->inicio == q->fim){
            aux = q->inicio;
            q->inicio = NULL; q->fim = NULL;
            free(aux);
            return 1;
        }
        else{
            aux = q->inicio;
            q->inicio = q->inicio->prox;
            q->inicio->ant = NULL;
            free(aux);
            return 1;
        }
    }
    if(q->fim == laser){
        aux = q->fim;
        q->fim = q->fim->ant;
        q->fim->prox = NULL;
        free(aux);
        return 1;
    }
    atual = q->inicio; frente = q->inicio->prox;
    while(atual != NULL){
        if(atual != laser){
            atual = frente;
            frente = frente->prox;
        }
        else break;
    }
    if(atual == q->fim){
        return 0;
    }
    aux = atual;
    atual->ant->prox = frente;
    frente->ant = atual->ant;
    free(aux);
    return 1;
}

//FUNÇÃO PARA VERIFICAR SE UM LASER COLIDE COM UM ASTEROIDE
void verificaColisao(listalineardeAst *lAst, listalineardeLaser *lLaser, relatorio *r){
    struct noLaser *auxL, *auxL2;
    struct noAsteroide *auxA, *auxA2;
    int totalWidthLaser;
    int totalWidthAsteroid;
    int totalHeightAsteroid;
    int colisao = 0;

    auxL = lLaser->inicio;

    //VERIFICAR TODOS OS LASERS NA LISTA
    while(auxL!=NULL){
        //SE LASER FOR PARA FORA DA TELA ELE É REMOVIDO
        if (auxL->l.y < -30) {
			auxL2 = auxL->prox;
            removeeLaser(lLaser, auxL);
            auxL = auxL2;
        } else {
            //LASER SUBINDO NA TELA E EXIBIDO
            auxL->l.y -= 20;
            readimagefile("assets/laser.gif",auxL->l.x,auxL->l.y,auxL->l.x+100,auxL->l.y+140);

			auxA = lAst->inicio;
            colisao = 0;
            //VERIFICAR TODOS OS ASTEROIDES NA LISTA
			while (auxA != NULL && colisao != 1) {
			    //VER ALTURA E LARGURA DOS LASERS E ASTEROIDES
				totalWidthLaser = auxL->l.x + 100;
				totalWidthAsteroid = auxA->a.x + auxA->a.m.w;
				totalHeightAsteroid = auxA->a.y + auxA->a.m.h;

                //VERIFICA SE O LASER DE FATO ESTÁ COLIDINDO COM O ASTEROIDE
				if ((auxL->l.y <= totalHeightAsteroid) && (auxL->l.y >= auxA->a.y) &&
				   ((totalWidthLaser >= auxA->a.x)&&(auxL->l.x <= totalWidthAsteroid))) {
                    //LASER É REMOVIDO
                    auxL2 = auxL->prox;
                    removeeLaser(lLaser, auxL);
                    //ASTEROIDE PERDE 3 DE SUA RESISTÊNCIA
                    auxA->a.m.resistencia -= 3;
                    //SOM DE COLISÃO
                    PlaySound(TEXT("assets/explosion.wav"), NULL, SND_ASYNC);
                    //CASO O ASTEROIDE SEJA EXPLODIDO, PONTUAÇÃO AUMENTA E ASTEROIDE É REMOVIDO
                    if (auxA->a.m.resistencia <= 0) {
                        pontuacao += auxA->a.m.pontuacao;
                        auxA2 = auxA->prox;
                        removeeAsteroide(lAst, auxA);
                        r->astDestruidos++;
                        auxA = auxA2;
                        PlaySound(TEXT("assets/explosion_loud.wav"), NULL, SND_ASYNC);
                    } else {
                        auxA = auxA->prox;
                    }

                    colisao = 1;
				} else {
                    auxA = auxA->prox;
                }
			}
            if (colisao == 0)
			    auxL = auxL->prox;
            else
                auxL = auxL2;
		}
    }
}

//FUNÇÃO PARA CRIAR FILA DO RELATÓRIO
void createFr(filaRelat *f)
{
    f->inicio=NULL;
    f->fim=NULL;
}

//FUNÇÃO PARA INSERIR RELATÓRIO NA FILA
int  insertFr(filaRelat *f, relatorio r){
    struct noRelat *aux;
    aux = (struct noRelat*)malloc(sizeof(struct noRelat));
    if (aux==NULL)
        return (0);

    aux->r=r;
    aux->prox=NULL;
    if (f->inicio==NULL)
        f->inicio=aux;
    if (f->fim!=NULL)
        f->fim->prox=aux;

    f->fim=aux;
    return(1);
}

//FUNÇÃO PARA REMOVER RELATÓRIO DA FILA
int  removeFr(filaRelat *f, relatorio *r){
    struct noRelat *aux;
    if (f->inicio==NULL)
        return (0);

    aux=f->inicio;
    f->inicio=aux->prox;
    if (f->inicio == NULL)
        f->fim = NULL;
    *r=aux->r;
    free(aux);
    return(1);
}

//FUNÇÃO PARA VERIFICAR SE A FILA DE RELATÓRIOS TA VAZIA
int  isEmpty(filaRelat f){
    if (f.inicio == NULL)
        return 1;
    return 0;
}

//FUNÇÃO PARA LIMPAR BUFFER DO TECLADO
void clear_keyboard_buffer(void)
{
    int c = 0;
    while (c = getch()) {}
    return;
}
