% Generate a random matrix with elements with normal distributions
n = 10000;
density = 0.2;
A = sprand(n, n, density);
V = rand(n, 1);

%{
[I, J, X] = find(A);

I = I - 1;
J = J - 1;

[m, n] = size(A);
p = zeros(n + 1, 1);
i = zeros(nnz(A), 1);
x = zeros(nnz(A), 1);

%I
%J
%X

%p

for it=1:1:length(J)
    %p(ii + 1) = J(ii)
    for jj=J(it)+2:1:length(p)
        p(jj) = p(jj) + 1;
    end
    i(it) = I(it);
    x(it) = X(it);
    it
end

%p'
%i'
%x'

% Save the matrix size to a file
file = fopen('n.dat', 'w');
fprintf(file, "%d", n);
fclose(file);
% Save the matrix p to a file
writematrix(p, 'p.dat', 'Delimiter', 'tab');
% Save the matrix i to a file
writematrix(i, 'i.dat', 'Delimiter', 'tab');
% Save the matrix x to a file
writematrix(x, 'x.dat', 'Delimiter', 'tab');
%}
% Save the matrix b to a file
writematrix(V, 'v.dat', 'Delimiter', 'tab');