alpha = 0.05;
epsilon = 0.005;
N = 0.25 * ( norminv( 1 - alpha / 2 ) /  epsilon ) ^ 2;

lambda = 3 * 4;

fmax = 0.7;

function y = f (x)
    if x < 0 || x > 3
        y = 0;
    elseif  0 <= x && x <= 1
        y = 0.4 * x^3 - 0.6 * x^2 + 0.3;
    elseif 1 < x && x <= 2
        y = -1.2 * (x-1)^3 + 1.8 * (x-1)^2 + 0.1;
    elseif  2 < x && x <= 3
        y = 1.2 * (x-2)^3 - 1.8 * (x-2)^2 + 0.7;
    endif
endfunction

vectorOfTotalWeights = zeros(N, 1);

for k = 1:N

    numOfFish = poissrnd(lambda);

    for i = 1 : numOfFish

        X = 0;      % Inıt. X, the weight of the fish which has been caught randomly
        Y = fmax;   % Init. Y

        while Y > f(X)
            X = 3 * rand;    % X = (a + b - a) * rand -> where a = 0 and b = 3
            Y = fmax * rand; % Y = c * rand where c = fmax = 0.7
        end

        vectorOfTotalWeights(k) += X;
    end
end

part_a = mean(vectorOfTotalWeights > 25)
part_b = mean(vectorOfTotalWeights)
part_c = std(vectorOfTotalWeights)
