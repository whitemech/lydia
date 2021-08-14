# History

## 0.1.3 (2021-08-14)

*    Fix building issue: ([#114](https://github.com/whitemech/lydia/issues/114))
*    Fix translation of Release LTLf-to-LDLf ([https://github.com/whitemech/lydia/commit/2070ec3608c68b7c7099c6eca89f4fde11ca0c95](https://github.com/whitemech/lydia/commit/2070ec3608c68b7c7099c6eca89f4fde11ca0c95))
*    Update Syft+ dependency version to v0.1.1 ([#119](https://github.com/whitemech/lydia/pull/119))

## 0.1.2 (2021-06-20)

*   Fix bug in the computation of the concatenation of two DFAs 
    ([#111](https://github.com/whitemech/lydia/pull/111)). 

## 0.1.1 (2021-06-07)

*   Made the parser compliant with
    [this standard](https://marcofavorito.me/tl-grammars/v/7d9a17267fbf525d9a6a1beb92a46f05cf652db6/).

*   Done minor changes to the source code. 

## 0.1.0 (2021-04-16)

First release of the software.

Main features:

*   Implementation of compositional approach for LDLf-to-DFA and LDLf synthesis
    ([paper](https://marcofavorito.me/papers/2021/degiacomo2021compositional.pdf))

*   Support for LTLf by first translating formulae into LDLf.

*   CLI tool to interact with the library.
