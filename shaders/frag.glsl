#version 450 core

// input
in vec3 FragPos;
in vec3 fNorm;
in vec2 fTexC;
uniform sampler2D ourTexture;

layout (location = 3) uniform vec3 ambientLGT; // color
layout (location = 4) uniform float ambientSTR;
layout (location = 6) uniform float diffuseSTR;
layout (location = 5) uniform vec3 diffusePOS;
layout (location = 7) uniform vec3 viewPos;

// output
out vec4 fragColor;

void main()
{
    /*
    ambient --> di base si moltiplicano tutti i fragmenti di un oggetto con un certo colore e un valore di intensità dando
                dando così l'effetto di luce ambientale/globale
    */

    vec3 amb = ambientLGT * ambientSTR;

    /*
    diffuse --> il prodotto scalare tra la norma (direzione della superfice) e la direzione della luce
                da un un numero 0 <= x <= 1, che andrà a moltiplicare il fragment, eseguendolo su tutti i fragment da
                l'effetto di luce diffusa (source light) creando zone più o meno ombrose
    */
    vec3 norm = normalize(fNorm);
    vec3 lightDir = normalize(diffusePOS - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * ambientLGT;

    /* 
    specular --> si calcola la luce riflessa rispetto ad ogni fragment, con quella luce riflessa si fa il prodotto scalare con 
                 la direzione in cui l'osservatore guarda, ciò ritorna un numero compreso tra 0 <= x <= 1 in base
                 alla direzione in cui l'osservatore guarda
    */
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  // reflect è una funzione che riflette un vettore rispetto alla superfice normale
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = diffuseSTR * spec * ambientLGT;
    
    // infine si sommano tutte le luci risultanti e si moltiplica per il colore dell'oggetto (texture)
    fragColor = texture(ourTexture, fTexC) * vec4(amb + diffuse + specular, 1.0f);

    /*
    material --> è la parametrizzazione dell'intensità, colore e riflesso della luce, ogni materiale ha queste proprietà
                che quando usate nella shader creano effetti differenti (materiali opachi, lucidi...)
    */
} 