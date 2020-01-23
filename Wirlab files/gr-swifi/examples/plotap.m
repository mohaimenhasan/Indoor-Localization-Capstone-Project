function plotap(samples)
%PLOTAP Plot amplitude and phase of input samples on two subplots.
%   The amplitude subplot is on top and the phase subplot is at the bottom.

    figure;
    subplot(2,1,1);
    plot(abs(samples));
    subplot(2,1,2);
    plot(angle(samples));
end

