
const SERVICE_UUID = '6e400001-b5a3-f393-e0a9-e50e24dcca9e';
const CHARACTERISTIC_UUID_RX = '6e400002-b5a3-f393-e0a9-e50e24dcca9e';
const CHARACTERISTIC_UUID_TX = '6e400003-b5a3-f393-e0a9-e50e24dcca9e';

var service
var writeCharacteristic;
var notifyCharacteristic;
var isConnected = false;
var isReady = true;
var isRunning = false;

var speed = 0.0;
var angle = 0.0;

function connect(){
    navigator.bluetooth.requestDevice({
        optionalServices: [SERVICE_UUID],
        acceptAllDevices: true
    }).then(device => {
            return device.gatt.connect();
    }).then(server => {
        return server.getPrimaryService(SERVICE_UUID);
    }).then(srv => {
        service = srv
        return service.getCharacteristic(CHARACTERISTIC_UUID_RX);
    }).then(characteristic => {
        writeCharacteristic = characteristic;
        console.debug(writeCharacteristic);
        return service.getCharacteristic(CHARACTERISTIC_UUID_TX);
    }).then(characteristic => {
        notifyCharacteristic = characteristic;
        console.debug(notifyCharacteristic);
        isConnected = true;
    });
}

function send_control_signal(speed, angle)
{
    return new Promise((resolve) => {
        if (!isConnected)
        {
            setTimeout(()=>{
                console.error("Device is not connected now.");
                resolve();
            }, 100);
        }

        var arrayBuffer = new ArrayBuffer(9);
        var dataView    = new DataView(arrayBuffer);
        dataView.setUint8(0, 0);
        dataView.setFloat32(1, speed, true);
        dataView.setFloat32(5, angle, true);

        isReady = false;
        writeCharacteristic.writeValue(arrayBuffer);
        setTimeout(()=>{
            isReady = true;
            resolve();
        }, 100);
    })
}

function loop(){
    console.log("loop");
    isRunning = true;
    send_control_signal(speed, angle).then(()=> {
        if(isRunning){
            loop();
        }
    });
}