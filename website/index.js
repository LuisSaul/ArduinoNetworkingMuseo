
//Declaración de varilabes
/** Variable que ayudará a dibujar sobre el lienzo canvas */
let ctx = document.getElementById("chart").getContext('2d');
/** Variables para los datos de los sensores (para gráficar) */
let temp = [];
let lum = [];
let dist = [];
let label = [];
let times = 0;

/** Chart, es la variable que nos ayuda con la gestión de
 * el lienzo canvas y ayuda a interpretar, acutalizar y
 * eliminiar valores */
let chart; 

/** Campos necesarios para acutalizar los valores que están en 
 * curso en la tabla superiror*/
let viewTemp = $('#temp');
let viewLumi = $('#lumi');
let viewDist = $('#dist');

/** Mensajes de alerta para cada una de las alertas del sistema */
let alertTemp = $('#alert-temp'); alertTemp.hide();
let alertLumi = $('#alert-lumi'); alertLumi.hide();
let alertDist = $('#alert-dist'); alertDist.hide();

/** Botones para prender y apagar el vetilador */
let on = $('#on-venti');
let off = $('#off-venti');

/** Creamos la gŕafica  y obtenemos los primeros valores de los 
 * sensores */
createChart();
getSensorData();

/** Función que inicializa el canvas con una gráfica */
function createChart(){
    chart = new Chart(ctx, {
        type: 'line',
        data: {
            labels:label,
            datasets: [{
                label: 'Temperatura',
                data: temp,                
                borderWidth: 1,
                fill: false,
                borderColor: 'rgb(255, 60, 0)',  
                backgroundColor: 'rgb(255, 60, 0)', 
                lineTension: 0.1
            },
            {
                label: 'Luminosidad',
                data: [6, 9, 6, 10, 4, 6],                
                borderWidth: 1,
                fill: false,
                borderColor: 'rgb(73, 187, 166)',  
                backgroundColor: 'rgb(73, 187, 166)', 
                lineTension: 0.1                
            },{
                label: 'Distancia',
                data: dist,  
                borderWidth: 1,
                fill: false,
                borderColor: 'rgb(228, 255, 0)',  
                backgroundColor: 'rgb(228, 255, 0)', 
                lineTension: 0.1
            }]
        },
        options: {
            scales: {
                yAxes: [{
                    ticks: {
                        beginAtZero:true
                    }
                }]
            }
        }
    });
}

/** Función que se utiliza para llamar al servicio web
 * y ayuda también a enviar información al arudino */
function getSensorData( data){
    $.ajax({
        url: 'http://192.168.1.177',
        method: 'GET',
        dataType: 'JSON',
        data:{
            data1: data
        },
        success: ( r ) =>{  
            console.log( r );          
            pushNewData(r.temp, r.lumels, r.distancia)
            showAlerts(r.tipo_alarma);
        },
        failure: ( error ) => {
            console.error( error );
        }
    });
}

/** Esta función permiete que sólo se mantengas 10 datos
 * en la gráfica */
function pushNewData(temperatura, luminosidad, distancia){
    viewTemp.html(temperatura)
    viewLumi.html(luminosidad)
    viewDist.html(distancia)
    if( chart.data.labels.length > 10 ){
        chart.data.labels.shift();
        chart.data.datasets[0].data.shift();
        chart.data.datasets[1].data.shift();
        chart.data.datasets[2].data.shift();
    }
    
    chart.data.datasets[0].data.push(temperatura);
    chart.data.datasets[1].data.push (luminosidad);
    chart.data.datasets[2].data.push(distancia);
    chart.data.labels.push( ++times );
    chart.update();
}

/** Esta función evalua el número y con respecto a sus bites
 * se muestran o esconden las alertas */
function showAlerts( numero ){
    numero = parseInt( numero );
    switch( numero ){
        case 0: 
            alertTemp.hide()
            alertLumi.hide()
            alertDist.hide()
        break;
        case 1: 
            alertTemp.hide();
            alertLumi.hide();
            alertDist.show();
        break;
        case 2: 
            alertTemp.hide();
            alertLumi.show();
            alertDist.hide();
        break;
        case 3: 
            alertTemp.show();
            alertLumi.hide();
            alertDist.hide();
        break;
        case 4: 
            alertTemp.hide();
            alertLumi.show();
            alertDist.show();
        break;
        case 5: 
            alertTemp.show();
            alertLumi.hide();
            alertDist.show();
        break;
        case 6: 
            alertTemp.show();
            alertLumi.show();
            alertDist.hide();
        break;
        case 7: 
            alertTemp.show();
            alertLumi.show();
            alertDist.show();
        break;
    }
}

/** Se declara una función que trae los datos periodicamente */
setInterval(() => {

    //getSensorData();


    /** Datos ficticios */
     pushNewData(
         Math.ceil(Math.random(100)*100),
         Math.ceil(Math.random(100)*100),
         Math.ceil(Math.random(100)*100)
     )
     showAlerts(Math.ceil((Math.random(6)*10) % 7));
},5000)

/** Enviamos 1 para prender el ventilador */
on.click(( e ) => {
    getSensorData(1);
})

/** Enviamos 0 para apagar el ventilador */
off.click(( e ) => {
    getSensorData(0);
})