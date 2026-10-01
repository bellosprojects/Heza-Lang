const DOCS_FULL = {
    "Basico": [{
        "name": "Comentarios",
        "description": "Son textos informativos que no se ejecutan"
    },
    {
        "name": "Variables",
        "description": "Actuan como cajas que guardan valores que usaremos despues"
    },
    {
        "name": "Salida",
        "description": "De este modo imprimimos en la terminal un resultado"
    },{
        "name": "Entrada",
        "description": "Te permite ingresar uno o mas valores y almacenarlos en variables"
    },{
        "name": "Funciones",
        "description": "Envuelven un bloque de codigo para reutilizarlo, puede tomar valores al momento de llamarlo para que haga una accion distinta"
    }, {
        "name": "Operadores",
        "description": "Los operadores permiten combinar valores para obtener resultados mas avanzados o simplificados, Heza cuenta con muchos operadores que debes conocer para sacar el maximo provecho"
    }],
    "Control": [{
        "name": "Condicionales",
        "description": "Nos ayudan a tomar desiciones, ejecutan una serie de instrucciones distintas dependiendo de la condicion aplicada"
    },{
        "name": "Bucle While",
        "description": "Repite un bloque de codigo mientras se cumpla la condicion declarada"
    }],
    "Estructuras": [{
        "name": "Objetos",
        "description": "Son como cajas que permiten almacenar varios valores y ponerles nombre a cada uno, de este modo se modelan mejor los algoritmos"
    }],
    "Funciones Matematicas": [{
        "name": "Funcion Pura",
        "description": "Una funcion de n parametros comun de matematicas"
    },{
        "name": "Funcion Trozos",
        "description": "Es una funcion que puede tomar distintos comportamiento dependiendo del valor de los parametros"
    }],
    "Expresiones Simbolicas": [{
        "name": "Expresiones",
        "description": "Son un objeto simbolico, es decir, que no se evalua al momento si no que funcionan de manera abstracta hasta que se les proporcione un valor"
    },{
        "name": "Derivadas Simbolicas",
        "description": "Evaluan la derivada de una Expresion con respecto a la variable especificada"
    },{
        "name": "Evaluacion Simbolica",
        "description": "Sustituye una variable simbolica por un valor (que incluso puede ser otra variable simbolica)"
    },{
        "name": "Limites",
        "description": "Evalua el limite de una expresion cuando una variable tiende a un valor especificado"
    }],
    "Conjuntos": [{
        "name": "Conjuntos Extension",
        "description": "Declaras uno a uno los elementos que pertenecen al conjunto"
    }, {
        "name": "Rangos",
        "description": "Definen rangos de numeros que avanzan unidades discretas para no escribir todos los numeros manualmente, tambien pueden tener un incremento configurable"
    },{
        "name": "Bucle For",
        "description": "Ejecutan una accion para cada elemento perteneciente a un conjunto"
    },{
        "name": "Filtros",
        "description": "Aplican una funcion a cada elemento de un conjunto y se queda con los que pasen la condicion"
    },{
        "name": "Mapeo",
        "description": "Para cada elemento de un conjunto se aplica una funcion y se almacena esa imagen en el nuevo conjunto. Adicionalmente, se puede agregar una condicion (como el filtro)"
    }],
    "Funciones Integradas": [{
        "name": "Matematicas",
        "description": "Conjutno de funciones matematicas integradas listas para usarse"
    },{
        "name": "Utiles",
        "description": "Conjunto de funciones integradas que con frecuencia son de utilidad"
    }]
};

export { DOCS_FULL };

export async function getCode(name){
    const path = `../documentation/${name}.hz`;

    const file = await fetch(path);
    const data = await file.text();

    return data;
}