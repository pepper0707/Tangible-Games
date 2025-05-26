# 1. Descriptions of experiments carried out in class.

We began by conducting a series of experiments to familiarise ourselves with Arduino hardware and fundamental electronics principles. This included learning how to deliver the correct voltage (3.3V or 5V) to components and using resistors to regulate current. These initial experiments introduced us to a wide range of components, which was essential in helping us think creatively about how they could be incorporated into our game.   

Additionally, many of our experiments involved using multiple components in conjunction, which required us to learn how to effectively manage and organize complex on a breadboard. Once we developed this foundational knowledge, it significantly expanded the potential scope and scale of our game, as we realized the potential to combine individual components to create more complex and interactive systems. 

## Basic LED/Potentiometer Experiment
Early on our experiments were basic such as when we used a potentiometer to control the brightness of an LED. While simple, it got us thinking about rotation as a key mechanic and also highlighted the use of a potentiometer for variable resistance alongside other components such as a speaker, for volume control. 

## Find Max Light Experiment
One particularly successful experiment involved combining a servo motor with a Light Dependent Resistor (LDR). We mounted the LDR on the servo and programmed a sweep across the motor’s range using the Arduino IDE. During this sweep, we continuously read values from the LDR via an analog pin, updating variables for both the light intensity and corresponding servo position. When the sweep completed, we could rotate the servo to point toward the brightest light source. 

![LDR/Servo Experiment](https://github.com/pepper0707/Tangible-Games/blob/formatting/Development%20Portfolio/01%20-%20Experiments%20carried%20out%20in%20class/FindMaxLight/images/00.jpg)

## LED Strip Experimentation
A defining experiment in shaping our game was testing an LED strip. After successfully connecting it to our Arduino, we explored various functions from the NeoPixel library, such as fading effects, chasing lights, and other patterns that dynamically altered colours and LED positions. Our team was especially enthusiastic about incorporating the LED strip into our game, more so than any other component, as we saw a wide range of opportunities to use it for visually engaging and responsive player feedback.

![LED Strip Experiment](https://github.com/pepper0707/Tangible-Games/blob/main/Development%20Portfolio/01%20-%20Experiments%20carried%20out%20in%20class/01.jpg)

## Statement of attribution

- Chloe and Alex: Helped code and put together FindMaxlight experiment
- All Team Members: Helped figure out wiring for Led/Potentiometer experiment
