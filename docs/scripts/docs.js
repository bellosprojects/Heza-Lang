// Mobile Sidebar Toggle
const mobileBtn = document.getElementById('mobile-menu-btn');
const sidebar = document.getElementById('sidebar');

if (mobileBtn && sidebar) {
    mobileBtn.addEventListener('click', () => {
        sidebar.classList.toggle('-translate-x-full');
    });
}

// Close sidebar on mobile when a link is clicked
const navLinks = document.querySelectorAll('.nav-link');
navLinks.forEach(link => {
    link.addEventListener('click', () => {
        if (window.innerWidth < 1024) { // lg breakpoint in tailwind
            sidebar.classList.add('-translate-x-full');
        }
    });
});

import { DOCS_FULL, getCode } from "./full_docs.js";

const docs_container = document.getElementById('docs-container');
const nav_bar = document.getElementById('nav-bar');

import { paintAllCodes } from "./lexer.js";

async function renderizarDocumentacion() {

    const promesasCategorias = Object.entries(DOCS_FULL).map(([tag, docs]) => {

        const TAG_DIV = document.createElement('div');
        const TAG_TITLE = document.createElement('h3');
        TAG_TITLE.className = 'font-heading font-semibold text-heza-cyan uppercase tracking-wider mb-4 text-[11px]';
        TAG_TITLE.innerText = tag;

        const ITEMS_NAV_LIST = document.createElement('ul');
        ITEMS_NAV_LIST.className = 'space-y-1 border-l border-heza-surface ml-2';

        TAG_DIV.appendChild(TAG_TITLE);
        TAG_DIV.appendChild(ITEMS_NAV_LIST);
        nav_bar.appendChild(TAG_DIV);

        const promesasTarjetas = docs.map(async (doc) => {
            const doc_name = doc.name;
            const description = doc.description;

            // --- Crear el NAV ---
            const NAV_ITEM = document.createElement('li');
            const A_NAV = document.createElement('a');
            A_NAV.className = 'nav-link block px-4 py-2 text-slate-400 hover:text-heza-cyan border-l-2 border-transparent transition-all';
            A_NAV.href = `#${doc_name.replaceAll(' ', '-')}`;
            A_NAV.innerHTML = doc_name;

            NAV_ITEM.appendChild(A_NAV);
            ITEMS_NAV_LIST.appendChild(NAV_ITEM);

            const code = await getCode(doc_name);

            // --- Crear la tarjeta ---
            const luxury_card = document.createElement('section');
            luxury_card.className = 'luxury-card p-6 md:p-10 space-y-6';
            luxury_card.id = doc_name.replaceAll(' ', '-');

            const header = document.createElement('div');
            header.className = 'border-b border-heza-teal/20 pb-4';

            const upper = document.createElement('div');
            upper.className = 'flex items-center gap-3 mb-2';

            const title = document.createElement('h2');
            title.className = 'text-2xl font-heading font-bold text-white';
            title.innerText = doc_name;

            const indicator = document.createElement('span');
            indicator.className = 'px-2 py-0.5 rounded text-[10px] font-mono border border-heza-teal/30 bg-heza-teal/10 text-heza-cyan';
            indicator.innerText = tag;

            upper.appendChild(title);
            upper.appendChild(indicator);

            const descText = document.createElement('p');
            descText.innerText = description;
            descText.className = 'text-heza-textMuted text-sm leading-relaxed';

            header.appendChild(upper);
            header.appendChild(descText);

            const code_container = document.createElement('div');
            code_container.className = 'code-container overflow-hidden';

            const windowIcons = document.createElement('div');
            windowIcons.className = 'code-header px-4 py-2.5 flex items-center justify-between';

            const icons = document.createElement('div');
            icons.className = 'flex gap-2';

            const redBall = document.createElement('div');
            redBall.className = 'w-3 h-3 rounded-full bg-red-500/80 border border-red-500/50';
            icons.appendChild(redBall);

            const yellowBall = document.createElement('div');
            yellowBall.className = 'w-3 h-3 rounded-full bg-yellow-500/80 border border-yellow-500/50';
            icons.appendChild(yellowBall);

            const greenBall = document.createElement('div');
            greenBall.className = 'w-3 h-3 rounded-full bg-green-500/80 border border-green-500/50';
            icons.appendChild(greenBall);

            windowIcons.appendChild(icons);
            code_container.appendChild(windowIcons);

            const editor = document.createElement('div');
            editor.className = 'p-5 flex-1 overflow-x-auto';

            const code_ = document.createElement('pre');
            const innerCode = document.createElement('code');
            code_.className = "heza-code";
            innerCode.innerHTML = code;

            code_.appendChild(innerCode);

            editor.appendChild(code_);
            code_container.appendChild(editor);

            luxury_card.appendChild(header);
            luxury_card.appendChild(code_container);

            // Devolvemos la tarjeta lista para ser insertada
            return luxury_card; 
        });

        return Promise.all(promesasTarjetas);
    });

    const tarjetasPorCategoria = await Promise.all(promesasCategorias);
    tarjetasPorCategoria.flat().forEach(tarjeta => docs_container.appendChild(tarjeta));

    paintAllCodes();
}

renderizarDocumentacion();

function initSmoothScroll(headerHeight = 60) {

    document.querySelectorAll('.nav-link').forEach(anchor => {
        anchor.addEventListener('click', function (e) {
            e.preventDefault();

            const targetId = this.getAttribute('href');
            const targetElement = document.querySelector(targetId);

            if (targetElement) {
                const elementPosition = targetElement.getBoundingClientRect().top + window.scrollY;
                
                const offsetPosition = elementPosition - headerHeight - 15;

                window.scrollTo({
                    top: offsetPosition,
                    behavior: 'smooth'
                });

                history.pushState(null, null, targetId);
            }
        });
    });
}

initSmoothScroll(60);