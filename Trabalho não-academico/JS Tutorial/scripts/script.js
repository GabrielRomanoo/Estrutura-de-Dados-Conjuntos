// const para = document.querySelector('#p1');
// para.addEventListener('click', updateName);

// function updateName() {
//     let name = prompt('Enter a new name.');
//     if (name != null) {
//         para.textContent = 'Player 1: ' + name;
//     }
//}

// function createParagraph() {
//     let para = document.createElement('p');
//    para.textContent = 'You clicked the button';
//    document.body.appendChild(para);
// }

// const buttons = document.querySelectorAll('button');

// for (let i = 0; i < buttons.length; i++) {
//     buttons[i].addEventListener('click', createParagraph);
// }

let randomNumber = Math.floor(Math.random() * 1000) + 1;
// floor, pega o primeiro int menor, ja ceil pega o maior int.
//O random gera um numero entre 0 e 1(nao pode ser 1, mas pode ser 0)(float)
//só que ai tipo, 0.99 * 100, 99, + 1 fora do bgl, ai vira 100 que é o max

const guesses = document.querySelector('.guesses');
const lastResult = document.querySelector('.lastResult');
const lowOrHi = document.querySelector('.lowOrHi');

const guessField = document.querySelector('.guessField');
const guessSubmit = document.querySelector('.guessSubmit');

let guessCount = 1;
let resetButton;
guessField.focus();

function checkGuess() {
    let userGuess = Number(guessField.value);

    if (guessCount === 1) {
        guesses.textContent = 'Previous guesses: ';
    }
    guesses.textContent += userGuess + ' ';

    if (userGuess === randomNumber) {
        lastResult.textContent = 'Congratulations! You got it bro';
        lastResult.style.backgroundColor = 'red';
        lowOrHi.textContent = '';
        setGameOver();
    } else if (guessCount == 5){
        lastResult.textContent = '|||GAME OVER|||';
        guessField.value = '';
        setGameOver();
    } else {
        lastResult.textContent = ('Wrong');
        lastResult.style.backgroundColor = 'black';
	lastResult.style.color = 'red';
        (userGuess < randomNumber) ? lowOrHi.textContent = 'Go Higher, mate' : lowOrHi.textContent = 'Go Lower, mate';
    }
    
    guessCount++;
    guessField.value = '';
    guessField.focus();
}

// guessSubmit.addEventListener('mouseover', checkGuess);
guessSubmit.addEventListener('click', checkGuess);

/* Faz com que dê pra submter com o ENTER */
guessField.addEventListener('keypress', function(event) {
    // event.preventDefault(); //Nao deixava eu escrever coisa
    if (event.keyCode == 13) {
        checkGuess();
    }
});

function setGameOver() {
    guessField.disabled = true;
    guessSubmit.disabled = true;
    resetButton = document.createElement('button');
    resetButton.textContent = 'Start a new game';
    document.body.appendChild(resetButton);
    resetButton.addEventListener('click', resetGame); // pode ser tbm resetButton.onclick = resetGame;
}

function resetGame() {
    guessCount = 1;

    const resetParas = document.querySelectorAll('.resultParas p');
    for (let i = 0; i < resetParas.length; i++) {
        resetParas[i].textContent = '';
    }

    resetButton.parentNode.removeChild(resetButton);

    guessField.disabled = false;
    guessSubmit.disabled = false;
    guessField.value = '';
    guessField.focus();

    lastResult.style.backgroundColor = 'white';

    randomNumber = Math.floor(Math.random() * 1000) + 1;
}