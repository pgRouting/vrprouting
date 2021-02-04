# vrpRouting - Documentation

## Publish on gh-Pages

vrpRouting documentation is hosted on [GitHub Pages](http://pages.github.com) with custom domain settings.

The following steps are needed to update the documentation:

1. Build documentation with CMake for example in `develop` branch
2. Checkout `gh-pages` branch
3. Copy the documentation from the `build` directory into a directory in root (ie. `dev`)
4. Update `index.html` if required, ie. paths.
6. Copy PDF file into version root directory.
7. Copy compressed MAN page into version root directory.
8. Commit and push the changes

The `index.html` documentation landing page needs to be edited manually.
It uses Sphinx template styles, but it is not generated automatically.

**Note:** This branch should be automaticc within github actions

The following are reminders on how to work manually

### for user's documentation

```
rm -Rf 0.0
cp -r build/doc/html 0.0
git add 0.0
```

### for developers documentation

Only for main branch documentation

```
rm -rf doxy
cp -r build/doxy/html doxygen
git add doxy
```

### for the pdf

    cp build/latex/pgRoutingDocumentation.pdf dev/
    gzip -c build/man/pgrouting.7 > dev/pgrouting.7.gz

    git add dev && cp build/index.html .

### commit with appropiate message
    git commit -m "updated to develop documentation for vrpRouting v0.0"
    git push origin gh-pages
