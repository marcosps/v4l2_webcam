all:
	flatpak-builder v4l_webcam_tmp org.test.V4l2Webcam.json --force-clean
	flatpak build-export v4l_repo v4l_webcam_tmp
	flatpak --user remote-add --no-gpg-verify --if-not-exists v4l_repo_test v4l_repo
	flatpak --user install v4l_repo_test org.test.V4lWebcam
	flatpak --user update org.test.V4lWebcam

clean:
	rm -rf v4l_repo v4l_webcam_tmp
