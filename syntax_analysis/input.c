int main() {
    int x = 1;
    int y = --x;
    while(x<5)
    {
        y = y % 3;
        y++;
        x++;
    }
    return y;
}