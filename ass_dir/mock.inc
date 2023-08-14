var a = 10;
var b = 10 + 10;
var c = 10 + 10 / 5;

function foo(args) {
    return args + 10;
}

function withLoop(args) {
    while (true) {
        if (a == 10) {
            a++;
            break;
        }
        else if (c == 10) {
            b--;
            return foo(b);
        }
        else {
            c--;
        }
    }
}

a = 5 + c && b;

LOG(a, b, c);