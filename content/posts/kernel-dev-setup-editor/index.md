---
title: "Setting up the kernel development environment - Editor"
description: How to setup the editor for basic Linux kernel development environment
date: 2023-01-24T20:23:05-08:00
featuredImage: "editor.jpg"
draft: false
toc:
  enable: true
tags: ["kernel", "setup", "editor", "emacs", "doom", "vim"]
series: [kernel-development-setup]
series_weight: 2
---

This article describes how to do the doom emacs setup for your kernel development
environment.
<!--more-->

## Overview
An important decision for the kernel development is the choice of which editor to pick.
The two most common choices are vim and emacs. There are certainly other choices
available. This guide installs both [emacs](https://www.gnu.org/software/emacs/) and
also [vim](https://www.vim.org/) and its variant [neovim](https://neovim.io/). The choice
of editor is always an opiniated one. For this series the
[doom emacs](https://github.com/doomemacs/doomemacs) variant of emacs
is used. Dooms emacs is an emacs configuration framework with vim key bindings. In
addition a lot of additional features are configured by default and make the life
of the developer easier.

## Package Installation
The first step is to install the required editor packages for arch Linux.
``` shell
sudo pacman -y -S vim
sudo pacman -y -S neovim
sudo pacman -y -S emacs-nativecomp
```
This doesn't install the standard emacs package, but instead the emacs package which
supports native compilation. Native compilation compiles the lisp packages that are
part of emacs or that are additionally installed. The result is faster execution of
these lisp packages and results in better response time.

In addition it can be beneficial to have additional search utilities installed. The
following two packages are recommended by the doom emacs installation and the last
one is one that I like.
``` shell
sudo pacman -y -S fd
sudo pacman -y -S ripgrep
sudo pacman -y -S the_silver_searcher
```

## Editor installation
The editor and search binaries are now installed. The next step is to install the
editor framework doom emacs. The following commands install the editor framework.
``` shell
git clone --depth 1 https://github.com/doomemacs/doomemacs ~/.emacs.d
~/.emacs.d/bin/doom install
```
As part of the installation a few questions need to be answered. The
[install guide](https://github.com/doomemacs/doomemacs/blob/master/docs/getting_started.org)
for doom emacs explains it in detail. To manage the packages and the doom emacs
installation the doom binary is used. It makes sense to add it to path of the
shell. For the [fish shell](https://fishshell.com/) this can be achieved with
the following command (other shells have simimilar commands: bash uses the
.bash_profile file).
``` shell
set -U fish_user_paths ~/.emacs.d/bin $fish_user_paths```
```

## Editor configuration changes
After making editor configuration changes it is required to update the editor
framework. Running the following doom command updates the editor configuration.
``` shell
doom sync
```

## Configuring the editor
### Basic module settings
The editor framework allows the user to enable and disable modules. The module
configuration is defined in the file init.el. To open the file press "Space f p"
and then select init.el.

The following shows which modules I have selected.
``` elisp
(doom! :input
       ;;bidi              ; (tfel ot) thgir etirw uoy gnipleh
       ;;chinese
       ;;japanese
       ;;layout            ; auie,ctsrnm is the superior home row

       :completion
       company           ; the ultimate code completion backend
       ;;helm              ; the *other* search engine for love and life
       ;;ido               ; the other *other* search engine...
       ;;ivy               ; a search engine for love and life
       vertico           ; the search engine of the future

       :ui
       ;;deft              ; notational velocity for Emacs
       doom              ; what makes DOOM look the way it does
       ;; doom-dashboard    ; a nifty splash screen for Emacs
       doom-quit         ; DOOM quit-message prompts when you quit Emacs
       ;;(emoji +unicode)  ; 🙂
       hl-todo           ; highlight TODO/FIXME/NOTE/DEPRECATED/HACK/REVIEW
       hydra
       ;;indent-guides     ; highlighted indent columns
       ;;ligatures         ; ligatures and symbols to make your code pretty again
       ;;minimap           ; show a map of the code on the side
       modeline          ; snazzy, Atom-inspired modeline, plus API
       nav-flash         ; blink cursor line after big motions
       ;;neotree           ; a project drawer, like NERDTree for vim
       ophints           ; highlight the region an operation acts on
       (popup +defaults)   ; tame sudden yet inevitable temporary windows
       ;;tabs              ; a tab bar for Emacs
       ;;treemacs          ; a project drawer, like neotree but cooler
       ;;unicode           ; extended unicode support for various languages
       (vc-gutter +pretty) ; vcs diff in the fringe
       vi-tilde-fringe   ; fringe tildes to mark beyond EOB
       ;;window-select     ; visually switch windows
       workspaces        ; tab emulation, persistence & separate workspaces
       ;;zen               ; distraction-free coding or writing

       :editor
       (evil +everywhere); come to the dark side, we have cookies
       file-templates    ; auto-snippets for empty files
       fold              ; (nigh) universal code folding
       ;;(format +onsave)  ; automated prettiness
       ;;god               ; run Emacs commands without modifier keys
       ;;lispy             ; vim for lisp, for people who don't like vim
       ;;multiple-cursors  ; editing in many places at once
       ;;objed             ; text object editing for the innocent
       ;;parinfer          ; turn lisp into python, sort of
       ;;rotate-text       ; cycle region at point between text candidates
       snippets          ; my elves. They type so I don't have to
       ;;word-wrap         ; soft wrapping with language-aware indent

       :emacs
       dired             ; making dired pretty [functional]
       electric          ; smarter, keyword-based electric-indent
       ;;ibuffer         ; interactive buffer management
       undo              ; persistent, smarter undo for your inevitable mistakes
       vc                ; version-control and Emacs, sitting in a tree

       :term
       ;;eshell            ; the elisp shell that works everywhere
       ;;shell             ; simple shell REPL for Emacs
       ;;term              ; basic terminal emulator for Emacs
       vterm             ; the best terminal emulation in Emacs

       :checkers
       syntax              ; tasing you for every semicolon you forget
       ;;(spell +flyspell) ; tasing you for misspelling mispelling
       ;;grammar           ; tasing grammar mistake every you make

       :tools
       ;;ansible
       ;;biblio            ; Writes a PhD for you (citation needed)
       ;;debugger          ; FIXME stepping through code, to help you add bugs
       ;;direnv
       ;;docker
       ;;editorconfig      ; let someone else argue about tabs vs spaces
       ;;ein               ; tame Jupyter notebooks with emacs
       (eval +overlay)     ; run code, run (also, repls)
       ;;gist              ; interacting with github gists
       lookup              ; navigate your code and its documentation
       lsp               ; M-x vscode
       magit             ; a git porcelain for Emacs
       ;;make              ; run make tasks from Emacs
       ;;pass              ; password manager for nerds
       ;;pdf               ; pdf enhancements
       ;;prodigy           ; FIXME managing external services & code builders
       ;;rgb               ; creating color strings
       ;;taskrunner        ; taskrunner for all your projects
       ;;terraform         ; infrastructure as code
       ;;tmux              ; an API for interacting with tmux
       ;;tree-sitter       ; syntax and parsing, sitting in a tree...
       ;;upload            ; map local to remote projects via ssh/ftp

       :os
       (:if IS-MAC macos)  ; improve compatibility with macOS
       ;;tty               ; improve the terminal Emacs experience

       :lang
       ;;agda              ; types of types of types of types...
       ;;beancount         ; mind the GAAP
       (cc +lsp)         ; C > C++ == 1
       ;;clojure           ; java with a lisp
       ;;common-lisp       ; if you've seen one lisp, you've seen them all
       ;;coq               ; proofs-as-programs
       ;;crystal           ; ruby at the speed of c
       ;;csharp            ; unity, .NET, and mono shenanigans
       ;;data              ; config/data formats
       ;;(dart +flutter)   ; paint ui and not much else
       ;;dhall
       ;;elixir            ; erlang done right
       ;;elm               ; care for a cup of TEA?
       emacs-lisp        ; drown in parentheses
       ;;erlang            ; an elegant language for a more civilized age
       ;;ess               ; emacs speaks statistics
       ;;factor
       ;;faust             ; dsp, but you get to keep your soul
       ;;fortran           ; in FORTRAN, GOD is REAL (unless declared INTEGER)
       ;;fsharp            ; ML stands for Microsoft's Language
       ;;fstar             ; (dependent) types and (monadic) effects and Z3
       ;;gdscript          ; the language you waited for
       ;;(go +lsp)         ; the hipster dialect
       ;;(graphql +lsp)    ; Give queries a REST
       ;;(haskell +lsp)    ; a language that's lazier than I am
       ;;hy                ; readability of scheme w/ speed of python
       ;;idris             ; a language you can depend on
       ;;json              ; At least it ain't XML
       ;;(java +lsp)       ; the poster child for carpal tunnel syndrome
       ;;javascript        ; all(hope(abandon(ye(who(enter(here))))))
       ;;julia             ; a better, faster MATLAB
       ;;kotlin            ; a better, slicker Java(Script)
       ;;latex             ; writing papers in Emacs has never been so fun
       ;;lean              ; for folks with too much to prove
       ;;ledger            ; be audit you can be
       ;;lua               ; one-based indices? one-based indices
       markdown          ; writing docs for people to ignore
       ;;nim               ; python + lisp at the speed of c
       ;;nix               ; I hereby declare "nix geht mehr!"
       ;;ocaml             ; an objective camel
       org               ; organize your plain life in plain text
       ;;php               ; perl's insecure younger brother
       ;;plantuml          ; diagrams for confusing people more
       ;;purescript        ; javascript, but functional
       ;;python            ; beautiful is better than ugly
       ;;qt                ; the 'cutest' gui framework ever
       ;;racket            ; a DSL for DSLs
       ;;raku              ; the artist formerly known as perl6
       ;;rest              ; Emacs as a REST client
       ;;rst               ; ReST in peace
       ;;(ruby +rails)     ; 1.step {|i| p "Ruby is #{i.even? ? 'love' : 'life'}"}
       ;;(rust +lsp)       ; Fe2O3.unwrap().unwrap().unwrap().unwrap()
       ;;scala             ; java, but good
       ;;(scheme +guile)   ; a fully conniving family of lisps
       sh                ; she sells {ba,z,fi}sh shells on the C xor
       ;;sml
       ;;solidity          ; do you need a blockchain? No.
       ;;swift             ; who asked for emoji variables?
       ;;terra             ; Earth and Moon in alignment for performance.
       ;;web               ; the tubes
       ;;yaml              ; JSON, but readable
       ;;zig               ; C, but simpler

       :email
       ;;(mu4e +org +gmail)
       mu4e
       ;;notmuch
       ;;(wanderlust +gmail)

       :app
       ;;calendar
       ;;emms
       ;;everywhere        ; *leave* Emacs!? You must be joking
       ;;irc               ; how neckbeards socialize
       ;;(rss +org)        ; emacs as an RSS reader
       ;;twitter           ; twitter client https://twitter.com/vnought

       :config
       ;;literate
       (default +bindings +smartparens))
```
Keep in mind with new versions of the editor framework more modules can become
avaiable or some modules might become obsolete.

### Additional packages
The editor framework allows the user to install additional lisp packages. I have
installed two additional packages: one for the theme that I use and the other one
is for the cscope installation. Additional packages can be specified in the file
packages.el. To open the file press "Space f p"
and then select packages.el.

Add the following lines in case you also want to add the theme and the cscope
integration.
``` elisp
(package! spacemacs-theme)
(package! consult-cscope :recipe (:host github :repo "blorbx/consult-cscope"))
```

### Configuration settings
The configuration of the settings can be changed in the file config.el.
To open the file press "Space f p" and then select config.el. All of the following
changes are made in the file config.el. If unsure where to make a specific change,
add the changes to the end of the file.

Set the user name and the email address. This is at the beginning of the file.
``` elisp
(setq user-full-name "John Doe"
      user-mail-address "your-email@your-domain.com")

```
In case you want to use the same theme make the following change.
``` elisp
;;(setq doom-theme 'doom-one)
(setq doom-theme 'spacemacs-dark)
```
I think line numbers are distracting so I disabled them.
``` elisp
;; (setq display-line-numbers-type t)
(setq display-line-numbers-type nil)
```
I also disabled the exit confirmation question.
``` elisp
;; Disable exit confirmation
(setq confirm-kill-emacs nil)
```

### Linux kernel coding style
Linux kernel development has its development process very well documented on the
[docs.kernel.org](https://docs.kernel.org) website. Part of the development process
is the [Linux kernel coding stype](https://docs.kernel.org/process/coding-style.html).
The linux kernel coding style defines among other things how the source code
needs to be formatted. This section describes how the editor is configured to
enforce this style guide.

The following changes make sure the correct settings are used.
``` elisp
;; Setting up linux kernel formatting options
;;
(defun linux-kernel-coding-style/c-lineup-arglist-tabs-only (ignored)
  "Line up argument lists by tabs, not spaces"
  (let* ((anchor (c-langelem-pos c-syntactic-element))
   (column (c-langelem-2nd-pos c-syntactic-element))
   (offset (- (1+ column) anchor))
   (steps (floor offset c-basic-offset)))
    (* (max steps 1)
       c-basic-offset)))

;; Add Linux kernel style
(add-hook 'c-mode-common-hook
    (lambda ()
      (c-add-style "linux-kernel"
       '("linux" (c-offsets-alist
            (arglist-cont-nonempty
             c-lineup-gcc-asm-reg
             linux-kernel-coding-style/c-lineup-arglist-tabs-only))))))

(defun linux-kernel-coding-style/setup ()
  (let ((filename (buffer-file-name)))
    ;; Enable kernel mode for the appropriate files
    (when (and buffer-file-name
               ( or (string-match "linux" buffer-file-name)
                    (string-match "liburing" buffer-file-name)))
                    ;; (string-match "xfstests" buffer-file-name)))
      (setq indent-tabs-mode t)
      (setq tab-width 8)
      (setq c-basic-offset 8)
      (c-set-style "linux-kernel"))))

(add-hook 'c-mode-hook 'linux-kernel-coding-style/setup)
```

### Configure LSP
To be able to use the compilation database LSP (language server protocol) needs
to be configured.
``` elisp
;; Configure LSP.
;;
(setq lsp-clients-clangd-args '("-j=3"
                                "--background-index"
                                "--clang-tidy"
                                "--completion-style=detailed"
                                "--header-insertion=never"
                                "--header-insertion-decorators=0"))
(after! lsp-clangd (set-lsp-priority! 'clangd 2))
(after! lsp-clangd (setq exec-path(append '("~/llvm-fb/9.0.0/bin/") exec-path)))
```
The last line is only required if the clangd executable is stored in a different
location.

### Configure cscope
To be able to use cscope to search for source code symbols, it needs to be
configured.
``` elisp
(setq consult-cscope-use-initial t)
(use-package! consult-cscope
  :defer t
  :commands (consult-cscope-symbol
             consult-cscope-definition
             consult-cscope-called-by
             consult-cscope-calling
             consult-cscope-text
             consult-cscope-egrep
             consult-cscope-file
             consult-cscope-including
             consult-cscope-assignment))

(map! :leader
 (:prefix-map ("c" . "code")
  (:prefix ("h" . "cscope")
   :desc "Search symbol" "s" #'consult-cscope-symbol
   :desc "Search definition" "d" #'consult-cscope-definition
   :desc "Search text" "t" #'consult-cscope-text
   :desc "Search file" "f" #'consult-cscope-file)))
```

### Configure git gutter
git gutter shows an indicator on the left side of the screen. For new lines it
shows a green bar, for deleted lines a red bar and for modified lines a yellow bar.
This configures the shapes associated with the current state.
``` elisp
;; Configure git gutter.
(custom-set-variables
 '(git-gutter:added-sign "█|")
 '(git-gutter:modified-sign "█⫶")
 '(git-gutter:deleted-sign "█▁"))

(after! git-gutter
  (set-face-foreground 'git-gutter:modified "yellow"))
```

### Overwrite some key bindings
The following overwrites some of the existing key bindings are adds new ones.
``` elisp
;; Overwrite existing key bindings.
;;
(map!
  (:leader
    (:prefix "b" :desc "Switch buffer" "b" 'consult-buffer)
    (:prefix "b" :desc "Switch workspace buffer" "B" '+vertico/switch-workspace-buffer)
    (:prefix "g" :desc "Format patch" "p" 'formatp-menu)))

(map!
 (:after evil-easymotion
  :m "gs" evilem-map
  (:map evilem-map
   "l" #'avy-goto-line)))
```

### Configure vterm
Emacs supports terminals. In this configuration emacs is using the vterm terminal.
To make sure that it uses our shell configuration, it sources the shell
configuration file.
``` elisp
(defun my/source-bashrc ()
      (interactive)
      (vterm-send-string "source ~/.bash_profile"))
(add-hook 'vterm-mode-hook #'my/source-bashrc)
```

### Configure whitespace mode
Sometimes it is difficult to see non-printable characters. This is what whitespace
mode is used for. It is not enable by default, but by enabling whitespace mode, it
show among other characters space and tab characters.
``` elisp
(defun my:see-all-whitespace () (interactive)
       (setq whitespace-style (default-value 'whitespace-style))
       (setq whitespace-display-mappings (default-value 'whitespace-display-mappings))
       (whitespace-mode 'toggle))
```
