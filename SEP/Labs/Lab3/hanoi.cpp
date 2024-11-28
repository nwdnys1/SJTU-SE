#include <iostream>
#include <string>
#include <climits>

#include "board.h"

using namespace std;

int main() {
    while (true) {
        cout << "How many disks do you want? (1 ~ 5)" << endl;
        char input;
        cin>>input;
        cin.get();
        if (input == 'Q') {
            break;
        }
        if ( input<='5'&&input>='1'){
            Board board(input-'0');//zhuchengxu
            board.draw();
            while(true){
                cout<<" Move a disk. Format: x y"<<endl;
                string in;
                getline(cin,in);
                if(in[1]=' '){
                    if(in[0]=='0'&&in[2]=='0') {
                        board.autoplay();
                        break;
                    }
                    else{
                        if(board.legal(in[0]-'0',in[2]-'0')){
                            //cout<<"legal"<<endl;
                            board.move(in[0]-'0',in[2]-'0',1);
                            if(board.win()) {
                                cout<<"Congratulations! You win!"<<endl;
                                break;
                            }
                            continue;
                        }
                        else{
                            //cout<<"illegal"<<endl;
                            board.draw();
                            continue;
                        }
                    }
                }
            }
        }
        else{

        }
        // TODO
    }
    return 0;
}
