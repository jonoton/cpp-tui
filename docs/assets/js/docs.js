(function () {
  'use strict';

  function initActiveNavScroll() {
    var navList = document.querySelector('.nav-list');
    var activeItem = document.querySelector('.nav-item.active');
    if (!navList || !activeItem) return;

    if (navList.scrollWidth > navList.clientWidth) {
      var targetLeft =
        activeItem.offsetLeft -
        navList.clientWidth / 2 +
        activeItem.offsetWidth / 2;
      navList.scrollLeft = Math.max(0, targetLeft);
    }
  }

  function initThemeToggle() {
    var btn = document.querySelector('.theme-toggle');
    if (!btn) return;
    var root = document.documentElement;
    var media = window.matchMedia('(prefers-color-scheme: dark)');
    function current() {
      return root.getAttribute('data-theme') || (media.matches ? 'dark' : 'light');
    }
    function label() {
      var next = current() === 'dark' ? 'light' : 'dark';
      btn.setAttribute('aria-label', 'Switch to ' + next + ' theme');
      btn.title = 'Switch to ' + next + ' theme';
    }
    btn.addEventListener('click', function () {
      var next = current() === 'dark' ? 'light' : 'dark';
      root.setAttribute('data-theme', next);
      try { localStorage.setItem('cpptui-theme', next); } catch (e) {}
      label();
    });
    if (media.addEventListener) media.addEventListener('change', label);
    label();
  }

  function initTableWrappers() {
    document.querySelectorAll('.doc-body :not(pre) > code').forEach(function (c) {
      if (c.textContent.length <= 32 && !c.closest('td:first-child')) c.classList.add('nowrap');
    });
    document.querySelectorAll('.doc-body td:first-child code').forEach(function (c) {
      if (c.textContent.length > 24 && c.textContent.indexOf('(') !== -1) c.classList.add('has-args');
    });
    var tables = document.querySelectorAll('.doc-body table');
    tables.forEach(function (table) {
      if (table.parentElement && table.parentElement.classList.contains('table-wrap')) {
        return;
      }
      var wrap = document.createElement('div');
      wrap.className = 'table-wrap';
      wrap.setAttribute('role', 'region');
      wrap.setAttribute('aria-label', 'Scrollable table');
      wrap.setAttribute('tabindex', '0');
      table.parentNode.insertBefore(wrap, table);
      wrap.appendChild(table);
    });

    // Fade the edge that has more columns to scroll to, so clipped tables
    // on phones show that they scroll sideways.
    function updateFade(el) {
      var max = el.scrollWidth - el.clientWidth;
      el.classList.toggle('fade-right', max > 2 && el.scrollLeft < max - 2);
      el.classList.toggle('fade-left', max > 2 && el.scrollLeft > 2);
    }
    var scrollers = document.querySelectorAll('.doc-body .table-wrap');
    scrollers.forEach(function (el) {
      updateFade(el);
      el.addEventListener('scroll', function () { updateFade(el); }, { passive: true });
    });
    window.addEventListener('resize', function () { scrollers.forEach(updateFade); });
  }

  function detectLanguage(container, pre, code) {
    var sources = [
      container ? container.className : '',
      pre ? pre.className : '',
      code ? code.className : ''
    ];
    for (var i = 0; i < sources.length; i++) {
      var match = sources[i].match(/\blanguage-([a-z0-9_+-]+)\b/i);
      if (match && match[1]) {
        var lang = match[1].toLowerCase();
        if (lang === 'plaintext' || lang === 'text' || lang === 'none') {
          return '';
        }
        return lang;
      }
    }
    return '';
  }

  function copyText(text) {
    if (navigator.clipboard && navigator.clipboard.writeText) {
      return navigator.clipboard.writeText(text);
    }
    return new Promise(function (resolve, reject) {
      try {
        var ta = document.createElement('textarea');
        ta.value = text;
        ta.setAttribute('readonly', '');
        ta.style.position = 'fixed';
        ta.style.top = '-9999px';
        ta.style.left = '-9999px';
        document.body.appendChild(ta);
        ta.select();
        var ok = document.execCommand('copy');
        document.body.removeChild(ta);
        if (ok) resolve();
        else reject(new Error('execCommand copy failed'));
      } catch (err) {
        reject(err);
      }
    });
  }

  function attachCodeHeader(container, pre, lang) {
    if (container.querySelector('.code-header')) return;

    var header = document.createElement('div');
    header.className = 'code-header';

    var langSpan = document.createElement('span');
    langSpan.className = 'code-lang';
    langSpan.textContent = lang || 'code';
    header.appendChild(langSpan);

    var btn = document.createElement('button');
    btn.type = 'button';
    btn.className = 'code-copy-btn';
    btn.textContent = 'Copy';
    btn.setAttribute('aria-label', 'Copy code to clipboard');

    var resetTimer = null;
    btn.addEventListener('click', function () {
      var codeEl = pre.querySelector('code');
      var raw = (codeEl ? codeEl.textContent : pre.textContent) || '';
      var cleaned = raw.replace(/\n$/, '');

      copyText(cleaned).then(function () {
        btn.textContent = 'Copied!';
        btn.classList.add('is-copied');
        if (resetTimer) clearTimeout(resetTimer);
        resetTimer = setTimeout(function () {
          btn.textContent = 'Copy';
          btn.classList.remove('is-copied');
        }, 1800);
      }).catch(function () {
        btn.textContent = 'Failed';
        if (resetTimer) clearTimeout(resetTimer);
        resetTimer = setTimeout(function () {
          btn.textContent = 'Copy';
        }, 1800);
      });
    });

    header.appendChild(btn);
    container.insertBefore(header, container.firstChild);
  }

  function initCodeBlocks() {
    var rougeBlocks = document.querySelectorAll('.doc-body div.highlighter-rouge');
    rougeBlocks.forEach(function (block) {
      var pre = block.querySelector('pre');
      if (!pre) return;
      var code = pre.querySelector('code');
      var lang = detectLanguage(block, pre, code);
      attachCodeHeader(block, pre, lang);
    });

    var standalonePres = document.querySelectorAll('.doc-body pre');
    standalonePres.forEach(function (pre) {
      if (pre.closest('.highlighter-rouge') || pre.closest('.code-block-wrap')) {
        return;
      }
      var wrap = document.createElement('div');
      wrap.className = 'code-block-wrap';
      pre.parentNode.insertBefore(wrap, pre);
      wrap.appendChild(pre);
      var code = pre.querySelector('code');
      var lang = detectLanguage(null, pre, code);
      attachCodeHeader(wrap, pre, lang);
    });
  }

  function collectContentHeadings() {
    var all = document.querySelectorAll('.doc-body h3[id], .doc-body h4[id]');
    var result = [];
    all.forEach(function (h) {
      if (h.closest('.doc-card') || h.closest('.api-toc') || h.closest('.hero')) {
        return;
      }
      result.push(h);
    });
    return result;
  }

  function initOnThisPageToc(headings) {
    var aside = document.getElementById('on-this-page');
    var nav = document.getElementById('toc-sidebar-nav');
    var shell = document.getElementById('page-shell');
    if (!aside || !nav || !shell || headings.length < 3) {
      return;
    }

    var list = document.createElement('ul');
    list.className = 'toc-list';

    var linkMap = Object.create(null);
    var groupOf = Object.create(null); // heading id -> owning h3 id
    var subItems = [];                 // level-4 <li> elements
    var currentGroup = null;
    // Long pages (the API reference) only expand the section being read.
    var accordion = headings.length > 40;
    if (accordion) aside.classList.add('is-accordion');

    headings.forEach(function (h) {
      var level = h.tagName.toLowerCase();
      var li = document.createElement('li');
      li.className = 'toc-item toc-level-' + level.charAt(1);
      if (level === 'h3') currentGroup = h.id;
      groupOf[h.id] = currentGroup;
      if (level === 'h4') {
        li.setAttribute('data-group', currentGroup || '');
        subItems.push(li);
      }

      var a = document.createElement('a');
      a.className = 'toc-link';
      a.href = '#' + h.id;
      a.textContent = h.textContent.trim();

      li.appendChild(a);
      list.appendChild(li);
      linkMap[h.id] = a;
    });

    nav.appendChild(list);
    aside.hidden = false;
    shell.classList.add('has-toc');

    var activeId = null;

    function setActive(id) {
      if (!id || id === activeId) return;
      if (activeId && linkMap[activeId]) {
        linkMap[activeId].classList.remove('is-active');
        linkMap[activeId].removeAttribute('aria-current');
      }
      activeId = id;
      if (accordion) {
        var group = groupOf[id];
        subItems.forEach(function (li) {
          li.classList.toggle('is-expanded', li.getAttribute('data-group') === group);
        });
      }
      var activeLink = linkMap[id];
      if (activeLink) {
        activeLink.classList.add('is-active');
        activeLink.setAttribute('aria-current', 'location');

        // Keep active TOC item visible inside sticky sidebar without moving window scroll
        var asideRect = aside.getBoundingClientRect();
        var linkRect = activeLink.getBoundingClientRect();
        if (linkRect.top < asideRect.top + 48) {
          aside.scrollTop -= asideRect.top + 48 - linkRect.top;
        } else if (linkRect.bottom > asideRect.bottom - 24) {
          aside.scrollTop += linkRect.bottom - (asideRect.bottom - 24);
        }
      }
    }

    function computeCurrentHeading() {
      var offset = (parseFloat(getComputedStyle(document.documentElement).scrollPaddingTop) || 80) + 16;
      var current = headings[0].id;
      for (var i = 0; i < headings.length; i++) {
        var top = headings[i].getBoundingClientRect().top;
        if (top <= offset) {
          current = headings[i].id;
        } else {
          break;
        }
      }
      setActive(current);
    }

    if ('IntersectionObserver' in window) {
      var observer = new IntersectionObserver(
        function () {
          computeCurrentHeading();
        },
        {
          rootMargin: '-80px 0px -65% 0px',
          threshold: [0, 1]
        }
      );
      headings.forEach(function (h) {
        observer.observe(h);
      });
    }

    var ticking = false;
    window.addEventListener(
      'scroll',
      function () {
        if (ticking) return;
        ticking = true;
        window.requestAnimationFrame(function () {
          computeCurrentHeading();
          ticking = false;
        });
      },
      { passive: true }
    );

    window.addEventListener('hashchange', function () {
      var hashId = window.location.hash ? window.location.hash.slice(1) : '';
      if (hashId && linkMap[hashId]) {
        setActive(hashId);
      }
    });

    computeCurrentHeading();
  }

  function initHeadingAnchors(headings) {
    headings.forEach(function (h) {
      if (h.querySelector('.heading-anchor')) return;
      var text = h.textContent.trim();
      var anchor = document.createElement('a');
      anchor.href = '#' + h.id;
      anchor.className = 'heading-anchor';
      anchor.setAttribute('aria-label', 'Link to section: ' + text);
      anchor.textContent = '#';
      h.appendChild(anchor);
    });
  }

  function initCollapsibleToc() {
    // The in-page API table of contents is very long; start it collapsed on phones.
    if (!window.matchMedia || !window.matchMedia('(max-width: 768px)').matches) return;
    document.querySelectorAll('.doc-body details.api-toc[open]').forEach(function (d) {
      d.removeAttribute('open');
    });
  }

  // ------------------------------------------------------------------------
  // API search: filter widgets and their members (properties, methods, callbacks)
  // ------------------------------------------------------------------------
  function initApiSearch() {
    var toc = document.querySelector('.doc-body details.api-toc');
    if (!toc) return;

    // Build an index of every h3/h4 section: title, category, member names, text.
    var heads = document.querySelectorAll('.doc-body h3[id], .doc-body h4[id]');
    var entries = [];
    var category = '';
    heads.forEach(function (h) {
      if (h.classList.contains('api-section-header')) {
        category = h.textContent.trim();
        return;
      }
      var members = [];
      var text = '';
      var el = h.nextElementSibling;
      while (el && !/^H[1-4]$/.test(el.tagName)) {
        el.querySelectorAll('td:first-child code, li > code:first-child').forEach(function (c) {
          c.textContent.split(/\s*\/\s*/).forEach(function (m) {
            m = m.trim();
            if (m && members.indexOf(m) === -1) members.push(m);
          });
        });
        text += ' ' + el.textContent;
        el = el.nextElementSibling;
      }
      entries.push({
        id: h.id,
        title: h.textContent.trim(),
        category: h.tagName === 'H3' ? 'Overview' : category,
        members: members,
        text: text.toLowerCase()
      });
    });

    var box = document.createElement('div');
    box.className = 'api-search';
    box.innerHTML =
      '<label class="api-search-field">' +
      '<svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" stroke-width="2" ' +
      'stroke-linecap="round" aria-hidden="true"><circle cx="11" cy="11" r="7"/><path d="M20 20l-3.5-3.5"/></svg>' +
      '<span class="visually-hidden">Search the API</span>' +
      '<input type="search" autocomplete="off" spellcheck="false" ' +
      'placeholder="Search widgets and members…" aria-controls="api-search-results">' +
      '<kbd class="api-search-key" aria-hidden="true">/</kbd></label>' +
      '<p class="api-search-status" aria-live="polite"></p>' +
      '<ul class="api-search-results" id="api-search-results" role="listbox" hidden></ul>';
    toc.parentNode.insertBefore(box, toc);

    var input = box.querySelector('input');
    var results = box.querySelector('.api-search-results');
    var status = box.querySelector('.api-search-status');
    var selected = 0;
    var current = [];

    function escapeHtml(s) {
      return s.replace(/[&<>"]/g, function (c) {
        return { '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c];
      });
    }

    function search(q) {
      q = q.trim().toLowerCase();
      if (!q) return [];
      var scored = [];
      entries.forEach(function (e) {
        var title = e.title.toLowerCase();
        var score = 0;
        var hits = e.members.filter(function (m) { return m.toLowerCase().indexOf(q) !== -1; });
        if (title === q) score = 100;
        else if (title.indexOf(q) === 0) score = 80;
        else if (title.indexOf(q) !== -1) score = 60;
        else if (hits.length) score = 40;
        else if (e.category.toLowerCase().indexOf(q) !== -1) score = 30;
        else if (e.text.indexOf(q) !== -1) score = 10;
        if (score) scored.push({ e: e, score: score, hits: hits });
      });
      scored.sort(function (a, b) { return b.score - a.score; });
      return scored.slice(0, 12);
    }

    function render() {
      var q = input.value;
      current = search(q);
      selected = 0;
      if (!q.trim()) {
        results.hidden = true;
        status.textContent = '';
        toc.hidden = false;
        return;
      }
      toc.hidden = true;
      results.hidden = false;
      if (!current.length) {
        results.innerHTML = '';
        status.textContent = 'No matches for “' + q.trim() + '”.';
        return;
      }
      status.textContent = current.length + (current.length === 1 ? ' match' : ' matches') +
        ' — press Enter to jump, ↑ ↓ to choose.';
      results.innerHTML = current.map(function (r, i) {
        var hits = r.hits.slice(0, 4).map(function (m) {
          return '<code>' + escapeHtml(m) + '</code>';
        }).join(' ');
        return '<li role="option" id="api-res-' + i + '"' + (i === 0 ? ' aria-selected="true"' : '') + '>' +
          '<a href="#' + r.e.id + '"><span class="api-res-title">' + escapeHtml(r.e.title) + '</span>' +
          '<span class="api-res-cat">' + escapeHtml(r.e.category) + '</span>' +
          (hits ? '<span class="api-res-hits">' + hits + '</span>' : '') + '</a></li>';
      }).join('');
    }

    function highlight() {
      var items = results.querySelectorAll('li');
      items.forEach(function (li, i) {
        if (i === selected) {
          li.setAttribute('aria-selected', 'true');
          li.scrollIntoView({ block: 'nearest' });
        } else {
          li.removeAttribute('aria-selected');
        }
      });
    }

    function close() {
      results.hidden = true;
      status.textContent = '';
      toc.hidden = false;
    }

    function go(id) {
      var target = document.getElementById(id);
      if (!target) return;
      close();
      input.blur();
      history.pushState(null, '', '#' + id);
      target.scrollIntoView({ behavior: 'smooth', block: 'start' });
      flash(target);
    }

    input.addEventListener('input', render);
    input.addEventListener('focus', function () { if (input.value.trim()) render(); });
    document.addEventListener('click', function (ev) {
      if (!box.contains(ev.target)) close();
    });
    input.addEventListener('keydown', function (ev) {
      if (ev.key === 'ArrowDown' && current.length) {
        selected = Math.min(selected + 1, current.length - 1); highlight(); ev.preventDefault();
      } else if (ev.key === 'ArrowUp' && current.length) {
        selected = Math.max(selected - 1, 0); highlight(); ev.preventDefault();
      } else if (ev.key === 'Enter' && current.length) {
        go(current[selected].e.id); ev.preventDefault();
      } else if (ev.key === 'Escape') {
        input.value = ''; render(); input.blur();
      }
    });
    results.addEventListener('click', function (ev) {
      var a = ev.target.closest('a');
      if (!a) return;
      ev.preventDefault();
      go(a.getAttribute('href').slice(1));
    });

    // Press "/" anywhere on the page to jump to the search box.
    document.addEventListener('keydown', function (ev) {
      if (ev.key !== '/' || ev.ctrlKey || ev.metaKey || ev.altKey) return;
      var tag = (document.activeElement && document.activeElement.tagName) || '';
      if (/^(INPUT|TEXTAREA|SELECT)$/.test(tag)) return;
      ev.preventDefault();
      input.focus();
      input.select();
    });

    // On small screens the search box scrolls away, so offer a floating button
    // (shown by CSS only on narrow screens) that brings it back.
    var fab = document.createElement('button');
    fab.type = 'button';
    fab.className = 'api-search-fab';
    fab.setAttribute('aria-label', 'Search the API');
    fab.innerHTML =
      '<svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="2.2" ' +
      'stroke-linecap="round" aria-hidden="true"><circle cx="11" cy="11" r="7"/><path d="M20 20l-3.5-3.5"/></svg>';
    fab.addEventListener('click', function () {
      box.scrollIntoView({ block: 'center' });
      input.focus({ preventScroll: true });
      input.select();
    });
    document.body.appendChild(fab);
    if ('IntersectionObserver' in window) {
      new IntersectionObserver(function (items) {
        fab.classList.toggle('is-visible', !items[0].isIntersecting && items[0].boundingClientRect.top < 0);
      }).observe(box);
    }
  }

  // Briefly highlight a section after jumping to it.
  function flash(el) {
    el.classList.remove('is-flash');
    void el.offsetWidth;
    el.classList.add('is-flash');
    setTimeout(function () { el.classList.remove('is-flash'); }, 1600);
  }

  function initTargetFlash() {
    function onHash() {
      var id = decodeURIComponent(location.hash.slice(1));
      var el = id && document.getElementById(id);
      if (el && /^H[2-4]$/.test(el.tagName)) flash(el);
    }
    window.addEventListener('hashchange', onHash);
    if (location.hash) setTimeout(onHash, 300);
  }

  document.addEventListener('DOMContentLoaded', function () {
    initCollapsibleToc();
    initActiveNavScroll();
    initThemeToggle();
    initTableWrappers();
    initCodeBlocks();

    initApiSearch();
    initTargetFlash();
    var headings = collectContentHeadings();
    initOnThisPageToc(headings);
    initHeadingAnchors(headings);
  });
})();
