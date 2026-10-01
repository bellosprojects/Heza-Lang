export function paintAllCodes () {

    const commentRegex = /~(.*)/;
    const operatorsRegex = /([-+*\/^%<>=:!|&]|∀|∃|∈|∑|∞|∅|∆|•|⨉|⋂|√|∪|≠|⊂|→|←|∧|∨|¬|∫|∇|∄|∏|∉|\\)/;
    const keywordsRegex = /\b(if|while|return|fun|else|use|copy|do|stop|object|from|as)\b/;
    const constantsRegex = /\b(true|false|LINE|Sys|π|null|∅|out|in|Text|Set|Bool|Number|Expression|Tuple|Inf|Nah)\b/;
    const idsRegex = /[a-zA-Z]\w*/;
    const expressionRegex = /'(?:\\.|[^'\\])*'/;
    const stringRegex = /"(?:\\.|[^"\\])*"/;
    const numberRegex = /\d+(\.\d+)?([eE][+-]\d+)?/;
    const diferentialRegex = /\bd[a-zA-Z]\b/;
    const functionRegex = /\b[a-zA-Z_]\w*(?=\s*\()/;
    const specialRegex = /@([a-zA-Z]\w*)/;
    const specialRegex2 = /#([a-zA-Z]\w*)/;

    const patterns = {
        comment: commentRegex,
        number: numberRegex,
        string: stringRegex,
        expression: expressionRegex,
        operator: operatorsRegex,
        keyword: keywordsRegex,
        constant: constantsRegex,
        diferential: diferentialRegex,
        function: functionRegex,
        special: specialRegex,
        special2: specialRegex2,
        ids: idsRegex
    };

    const combinedRegexString = Object.entries(patterns)
        .map(([name, regex]) => `(?<${name}>${regex.source})`)
        .join('|');

    const masterRegex = new RegExp(combinedRegexString, 'g');

    const escapeHtml = (str) => str
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;");

    document.querySelectorAll('.heza-code').forEach(doc => {

        const code = doc.textContent;

        const highlighted = code.replace(masterRegex, (match, ...args) => {
            const groups = args[args.length - 1];

            for (const [name, value] of Object.entries(groups)) {
                if (value !== undefined) {
                    return `<span class="${name}">${escapeHtml(value)}</span>`;
                }
            }
            return match;
        });
        
        const finalHtml = highlighted.replace(/(<span[^>]*>.*?<\/span>)|([^<]+|<)/g, (_, isSpan, isPlain) => {
            if (isSpan) {
                return isSpan;
            }
            return escapeHtml(isPlain);
        });

        doc.innerHTML = finalHtml;

    });

}