# Controller
requirements:
- motor sensor driver
- motor control driver
- pid regulator
  - quality of regulator: no oscillations, fast response, minimal steady state error
  - should not be affected by FTP file transfer or http server
- set point for motor position should be possible to set through UDP
  - message format: non-clamped degrees value

# Web server
A website that displays charts generated from board data.

requirements:
- web page with live graphs
- actual motor position (absolute value)
- requested position (absolute value as received from the reference motor)
- current PWM duty cycle (in range –100%, +100%)
- The graphs should show at least 2 seconds of history with time resolution ≤ 5 ms. The
graph should be refreshed at least every 100 ms.

# Documentation
requirements:
- Data-flow diagram of application
  - should contain: IRQ handlers, controller, web server