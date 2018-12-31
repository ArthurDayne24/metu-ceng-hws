
setting=[0, 0.2, 10, 20];

no = load('no');
one = load('2-1');
two = load('2-2');
three = load('2-3');

mean_no = mean(no) / 2;
mean_one = mean(one) / 2;
mean_two = mean(two) / 2;
mean_three = mean(three) / 2;

values = [mean_no, mean_one, mean_two, mean_three];

% calculate error margin
% https://www.mathsisfun.com/data/confidence-interval.html

err_no = 1.96 * std(no) / sqrt(5);
err_one = 1.96 * std(one) / sqrt(5);
err_two = 1.96 * std(two) / sqrt(5);
err_three = 1.96 * std(three) / sqrt(5);

errors = [err_no, err_one, err_two, err_three];

figure1 = figure;
errorbar(setting, values, errors);
xlabel('Corruption Percentage (%)');
ylabel('File Transfer Time (s)');
title('File Transfer Time vs Corruption Percantage');
savefig(figure1, 'plot2.fig')
pause;

