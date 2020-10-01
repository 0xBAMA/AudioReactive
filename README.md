# AudioReactive
Experimenting with FFTs, using them to displace geometry, etc

Note that for FFT calculations, I am now requiring FFTW3 be installed on the system.

If you dig into the code here at any depth, you'll find that this project is using the same sort-of-WIP engine that I have been using for several projects, rtiow, rttnw, Voraldo 1.0 and 1.1 - the addition of very fast FFT capabilities with FFTW3 might prove to be interesting.

My intention going into this is to create surfaces representing different channels of audio - by layering these surfaces over one another, you could observe relative intensity at different frequency ranges and at very least judge which is greater, if they were represented with contrasting colors.
