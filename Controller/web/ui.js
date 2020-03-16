onload = function()
{
    let canvas = document.getElementById("canvas");
    canvas.style.height = this.window.height;
    canvas.style.width = this.window.width;

    let context = canvas.getContext('2d');
    let ctrlLoop;

    let img_rudder, img_throttle_base, img_throttle_lever;

    const promise_rudder = new Promise((resolve)=>{
        img_rudder = new Image();
        img_rudder.src = "./rudder.png";
        img_rudder.onload = () => resolve();
    });

    const promise_throttle_base = new Promise((resolve) => {
        img_throttle_base = new Image();
        img_throttle_base.src = "./throttle.png";
        img_throttle_base.onload = () => resolve();
    });

    const promise_throttle_lever = new Promise((resolve) => {
        img_throttle_lever = new Image();
        img_throttle_lever.src = "./throttle_lever.png";
        img_throttle_lever.onload = () => resolve();
    });

    function drawRudder(angle)
    {
        context.clearRect(0, 0, canvas.width, canvas.height);
        context.save();
        context.translate(canvas.width * 0.3, canvas.height * 0.5);
        context.rotate(angle);
        context.drawImage(img_rudder, -(img_rudder.width/2), -(img_rudder.height/2));
        context.restore();
    }

    function drawThrottle(speed)
    {
        return new Promise((resolve)=>{
            context.drawImage(img_throttle_base, canvas.width * 0.6, canvas.height * 0.1, canvas.width * 0.2, canvas.height * 0.8);
            resolve();
        }).then(()=>{
            context.drawImage(
                img_throttle_lever,
                canvas.width * 0.65,
                canvas.height * (0.5 + speed /2) - canvas.width * 0.025,
                canvas.width * 0.1,
                canvas.width * 0.05);
        });
    }

    function startLoop(){
        if(isConnected){
            loop();
        }
    }

    function stopLoop(){
        isRunning = false;
    }

    function draw(speed, angle)
    {
        drawRudder(angle);
        drawThrottle(speed);
    }

    function getTouchedPosition(e){
        const canvasRect = e.target.getBoundingClientRect();
        const x = (e.changedTouches[0].clientX - canvasRect.left) / canvasRect.width;
        const y = (e.changedTouches[0].clientY - canvasRect.top) / canvasRect.height;
        return [x, y];
    }

    function getCategory(x, y)
    {
        if(x < 0.5){
            return 1;
        } else {
            return 2;
        }
    }

    function update(e)
    {
        const CAT_RUDDER = 1;
        const CAT_THROTTLE = 2;
        let x, y, category;

        [x, y] = getTouchedPosition(e);
        category = getCategory(x, y);

        if (CAT_RUDDER == category)
        {
            console.log("rudder");
            angle = (x - 0.3) *180/Math.PI; 
        } else if(CAT_THROTTLE == category){
            console.log("throttle", speed);
            speed = - (y - 0.5) * 333;
            speed = (speed > 100) ? 100 : speed;
            speed = (speed < -100) ? -100 : speed;
        }
    }

    function updateCanvas()
    {
        draw(-speed * 0.005, angle * 0.3);   
    }

    function onTouchStart(e){
        update(e);
        startLoop();
    }

    function onTouchEnd(e){
        console.log("touchend");
        stopLoop();
        update(e);
    }

    function onTouchCancel(e){
        console.log("touchcancel");
        stopLoop();
    }

    function onTouchMove(e){
        console.log("touchmove");
        update(e);
    }

    // canvas.addEventListener('mousedown', onDown, false);
    // canvas.addEventListener('mouseup', onUp, false);
    // canvas.addEventListener('click', onClick, false);
    // canvas.addEventListener('mouseover', onOver, false);
    // canvas.addEventListener('mouseout', onOut, false);

    Promise.all([
        promise_rudder,
        promise_throttle_base,
        promise_throttle_lever]).then((resolve) => {
            draw(0, 0);
            resolve();
        });
    
    setInterval(updateCanvas, 50);

    canvas.addEventListener('touchstart', onTouchStart, false);
    canvas.addEventListener('touchend', onTouchEnd, false);
    canvas.addEventListener('touchcancel', onTouchCancel, false);
    canvas.addEventListener('touchmove', onTouchMove, false);

    let btnConnect = document.getElementById("btn-connect");
    btnConnect.addEventListener('click', connect, false);
}