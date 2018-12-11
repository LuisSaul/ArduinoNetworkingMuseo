let ctx = document.getElementById("chart").getContext('2d');
let temp = [];
let lum = [];
let dist = [];
let label = [];
let times = 0;
let chart; 

let viewTemp = $('#temp');
let viewLumi = $('#lumi');
let viewDist = $('#dist');

let alertTemp = $('#alert-temp'); alertTemp.hide();
let alertLumi = $('#alert-lumi'); alertLumi.hide();
let alertDist = $('#alert-dist'); alertDist.hide();

let on = $('#on-venti');
let off = $('#off-venti');

createChart();

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
setInterval(() => {
    getSensorData();
    // pushNewData(
    //     Math.ceil(Math.random(100)*100),
    //     Math.ceil(Math.random(100)*100),
    //     Math.ceil(Math.random(100)*100)
    // )
    // showAlerts(Math.ceil((Math.random(6)*10) % 7));
},15000)

on.click(( e ) => {
    getSensorData(1);
})

off.click(( e ) => {
    getSensorData(0);
})