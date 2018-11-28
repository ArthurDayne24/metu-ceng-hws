
emulated=[0, 1, 20, 60];

no = load('data/no');
one = load('data/1');
twenty = load('data/20');
sixty = load('data/60');

mean_no = mean(no) / 2;
mean_one = mean(one) / 2;
mean_twenty = mean(twenty) / 2;
mean_sixty = mean(sixty) / 2;

end_to_end = [mean_no, mean_one, mean_twenty, mean_sixty];

% calculate error margin
% https://www.mathsisfun.com/data/confidence-interval.html

err_no = 1.96 * std(no) / sqrt(1000);
err_one = 1.96 * std(one) / sqrt(1000);
err_twenty = 1.96 * std(twenty) / sqrt(1000);
err_sixty = 1.96 * std(sixty) / sqrt(1000);

errors = [err_no, err_one, err_twenty, err_sixty];

figure1 = figure;
errorbar(emulated, end_to_end, errors);
xlabel('Network Emulation Delay (ms)');
ylabel('End-to-End Delay (ms)');
title("Network Emulation Delay vs End-to-End Delay");
savefig(figure1, 'plot.fig')
pause;

